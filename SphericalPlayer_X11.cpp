/*********************************************************************************
 * Spherical Player for equirectangular video
 * GLFW_VERSION_MAJOR == 3
 **********************************************************************************/

typedef unsigned char BYTE;
typedef wchar_t WCHAR;

#include <iostream>
#include <string>
#include <fstream>
#include <stdlib.h>
#include <math.h>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#elif __LINUX
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/osmesa.h>
#include <GLFW/glfw3.h>

#include <assert.h>

#include "math/matrix4.h"
#include "math/vector3.h"
#include "cv/VideoFrame.h"
#include "ZoomInterpolation/BMPLoader.h"

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

#define SAVE_TARGA

#if GLFW_VERSION_MAJOR==3
GLFWwindow* g_window = NULL;
#endif
#ifndef GLFW_FALSE
#define GLFW_FALSE 0
#endif

#define XAXIS vector3(1,0,0)
#define YAXIS vector3(0,1,0)
#define ZAXIS vector3(0,0,1)

using namespace std;
using namespace cv;

string video_dir;
Player* player;


// Eye
/*
 * gluLookAt
 * eyex, eyey, eyez : camera position
 * centerx, centery, centerz : point camera is looking at
 * upx, upy, upz : camera upward vector
 */
double eyes[10][9] =
{
    {0,0,0,0,0,1,0,1,0},
    {0,0,0,-1,0,0,0,1,0},
    {0,0,0,0,0,-1,0,1,0},
    {0,0,0,1,0,0,0,1,0},
    {0,0,0,0,1,0,0,0,1},
    {0,0,0,1,-1,1,0,0,1},
    {0,0,0,1,1,-1,0,0,1},
    {0,0,0,1,-1,-1,0,0,1},
    {0,0,0,1,1,1,0,0,1},
    {120,50,180,0,0,0,0,1,0}
};
float angle = 0.0f, ratio;
float x, y, z;
float lx, ly, lz;

int eyeCount = sizeof(eyes) / sizeof(eyes[0]);
int eyeIndex;
vector<matrix4> wld2eye, eye2wld;

// Sphere
matrix4 sphere2wld;
int sphereID;
GLuint sphereText;
float Radius = 700;

typedef struct // struct for sphere
{
    double X;
    double Y;
    double Z;

    double U;
    double V;
}VERTICES;
const int space = 1;
const int vertexCount = (90/space)*(360/space)*4;
VERTICES VERTEX[vertexCount];


// Width and Height for video player window
int width, height;
static int isDrag = 0;

// Screen pixels
unsigned char* pixels;

/*********************************************************************************
 * OpenCV Mat to OpenGL texture
 **********************************************************************************/
GLuint MatToTexture(Mat _image)
{
    Mat image;
    _image.copyTo(image);
    if(image.empty()) return -1;
    cvtColor(image, image, CV_BGR2RGB);
    flip(image, image, 1);

    GLuint textureID;
    glGenTextures(1, &textureID);

    //Bind texture ID
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.cols, image.rows,
                 0, GL_RGB, GL_UNSIGNED_BYTE, image.ptr());

    return textureID;
}


/*********************************************************************************
 * Draw x, y, z axis of current frame on screen.
 * x, y, and z are corresponded Red, Green, and Blue, resp.
 **********************************************************************************/
void drawFrame(float len)
{
    glDisable(GL_LIGHTING);		// Lighting is not needed for drawing axis.
    glBegin(GL_LINES);			// Start drawing lines.
    glColor3d(1,0,0);			// color of x-axis is red.
    glVertex3d(0,0,0);
    glVertex3d(len,0,0);		// Draw line(x-axis) from (0,0,0) to (len, 0, 0).
    glColor3d(0,1,0);			// color of y-axis is green.
    glVertex3d(0,0,0);
    glVertex3d(0,len,0);		// Draw line(y-axis) from (0,0,0) to (0, len, 0).
    glColor3d(0,0,1);			// color of z-axis is  blue.
    glVertex3d(0,0,0);
    glVertex3d(0,0,len);		// Draw line(z-axis) from (0,0,0) - (0, 0, len).
    glEnd();					// End drawing lines.
}


/*********************************************************************************
 * Draw sphere mesh
 * Create sphere
 **********************************************************************************/
void drawSphere(double R, GLuint texture)
{
    glScalef (0.0125 * R, 0.0125 * R, 0.0125 * R);
    glRotatef (90, 1, 0, 0);
    glBindTexture (GL_TEXTURE_2D, texture);
    glBegin (GL_TRIANGLE_STRIP);

    for (int i = 0; i <= vertexCount; i++){
        glTexCoord2f (VERTEX[i].U, VERTEX[i].V);
        glVertex3f (VERTEX[i].X, VERTEX[i].Y, -VERTEX[i].Z);
    }

    for (int i = 0; i <= vertexCount; i++){
        glTexCoord2f (VERTEX[i].U, -VERTEX[i].V);
        glVertex3f (VERTEX[i].X, VERTEX[i].Y, VERTEX[i].Z);
    }
    glEnd();

}

void createSphere(double R, double H, double K, double Z)
{
    int n=0;

    for (double j=0; j<= 90-space; j+=space){
        for (double i=0; i<= 360-space; i+=space){
            // Start editing our vertex.
            // I am calculating the X value here.
            VERTEX[n].X = R * sin((i) / 180 * M_PI) * sin((j) / 180 * M_PI) - H;
            // The Y value here.
            VERTEX[n].Y = R * cos((i) / 180 * M_PI) * sin((j) / 180 * M_PI) + K;
            // The Z value here.
            VERTEX[n].Z = R * cos((j) / 180 * M_PI) - Z;
            // Now I am calculating the texture coordinates. I have used (2*j) as the texture
            // is twice as wide as it is high. Hence 2:1. You can remove the (2*) if you wish
            // to use i texture with the same width and height, or increase it accordingly.
            VERTEX[n].V = (2 * j) / 360;
            VERTEX[n].U = (i) / 360;
            // Then start working with the next vertex
            n++;


            // Then we do the same calculations as before, only adding the space variable
            // to the j values.
            VERTEX[n].X = R * sin((i) / 180 * M_PI) * sin((j + space) / 180 * M_PI) - H;
            VERTEX[n].Y = R * cos((i) / 180 * M_PI) * sin((j + space) / 180 * M_PI) + K;
            VERTEX[n].Z = R * cos((j + space) / 180 * M_PI) - Z;
            VERTEX[n].V = (2 * (j + space)) / 360;
            VERTEX[n].U = (i) / 360;
            n++;

            // Then we do the same calculations as the first, only adding the space variable
            // to the i values.
            VERTEX[n].X = R * sin((i + space) / 180 * M_PI) * sin((j) / 180 * M_PI) - H;
            VERTEX[n].Y = R * cos((i + space) / 180 * M_PI) * sin((j) / 180 * M_PI) + K;
            VERTEX[n].Z = R * cos((j) / 180 * M_PI) - Z;
            VERTEX[n].V = (2 * j) / 360;
            VERTEX[n].U = (i + space) / 360;
            n++;

            // Then we do the same calculations as the first again, only adding the space variable
            // to both the j and the i values.
            VERTEX[n].X = R * sin((i + space) / 180 * M_PI) * sin((j + space) / 180 * M_PI) - H;
            VERTEX[n].Y = R * cos((i + space) / 180 * M_PI) * sin((j + space) / 180 * M_PI) + K;
            VERTEX[n].Z = R * cos((j + space) / 180 * M_PI) - Z;
            VERTEX[n].V = (2 * (j + space)) / 360;
            VERTEX[n].U = (i + space) / 360;
            n++;
        }
    }
}


/*********************************************************************************
 * Call this part whenever display events are needed.
 * Display events are called in case of re-rendering by OS. ex) screen movement, screen maximization, etc.
 **********************************************************************************/
void display()
{
    Mat frame = player-> getNextFrame();

    // Call OpenCV to get next frame
    sphereText = MatToTexture(frame);
    //imshow("Sample Video", frame);

    // call display() for every frame of the video
    glClearColor(0., 0.25f, 1.0f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // set view transformation
    glLoadMatrixd(wld2eye[eyeIndex].GLmatrix());

    //glMatrixMode(GL_MODELVIEW);
    //glLoadIdentity();
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, sphereText);

    drawSphere(8, sphereText);

    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    /*bitmap_image image(width, height);
    image_drawer draw(image);

    for (unsigned int i = 0; i < image.width(); ++i)
    {
        for (unsigned int j = 0; j < image.height(); ++j)
        {
            image.set_pixel(i,j,*(++imageData),*(++imageData),*(++imageData));
        }
    }

    image.save_image("Trangle_image.bmp");
    */
    glFlush();
}


/*********************************************************************************
 * Call this part whenever size of the window is changed.
 **********************************************************************************/
void reshape( int w, int h)
{
    width = w;
    height = h;
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);            // Select The Projection Matrix
    glLoadIdentity();                       // Reset The Projection Matrix

    // Define perspective projection frustum
    double aspect = width / double(height);

    // void gluPerpective(GLdouble fovy, GLdouble aspect, GLdouble near, GLdouble far);
    gluPerspective(30.0f, aspect,  1, 1024);
    glMatrixMode(GL_MODELVIEW);             // Select The Modelview Matrix
    glLoadIdentity();                       // Reset The Projection Matrix
}


/*********************************************************************************
 * Call this part whenever mouse button is clicked.
 **********************************************************************************/
#if GLFW_VERSION_MAJOR==3
void onMouseButton(GLFWwindow* window, int button, int state, int __mods)
#else
void onMouseButton(int button, int state)
#endif
{
    const int GLFW_DOWN = 1;
    const int GLFW_UP = 0;
    int x, y;
#if GLFW_VERSION_MAJOR == 3
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    x = xpos;
    y = ypos;
#else
    glfwGetCursorPos(&x, &y);
#endif
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if(state == GLFW_DOWN)
        {
            isDrag = 1;
        }
        else if (state == GLFW_UP)
        {
            isDrag = 0;
        }
    }
}

/*********************************************************************************
 * Call this part whenever user drags mouse
 * Input x, y : coordinate of mouse
 **********************************************************************************/
#if GLFW_VERSION_MAJOR == 3
void onMouseDrag(GLFWwindow* window, double fx, double fy)
{
    int x = fx;
    int y = fy;
#else
void onMouseDrag( int x, int y)
{
#endif
    if (isDrag)
    {
        printf("Mouse Dragging\n");
    }
}

/*********************************************************************************
* Call this part whenever user types keyboard.
**********************************************************************************/
#if GLFW_VERSION_MAJOR==3
void onKeyPress(GLFWwindow *__win, int key, int __scancode, int action, int __mods)
#else
void onKeyPress( int key, int action)
#endif
{
    if(action == GLFW_RELEASE)
        return ;
    if ((key == ' ') || (key == 'r'))
    {
        printf("Viewpoint change %d\n", eyeIndex);
        eyeIndex = (eyeIndex + 1) % eyeCount;
    }

    else if (key == 'w'){

    }

    else if (key == 's'){

    }

    else if (key == 'a'){

    }

    else if (key == 'd') {

    }

}

/*********************************************************************************
* Load texure video from file
* (Equirectangular video)
**********************************************************************************/
void initialize()
{
    // Set up OpenGL state
    glShadeModel(GL_SMOOTH);  // Smooth shading
    glEnable(GL_DEPTH_TEST);  // Enable depth testing
    glEnable(GL_TEXTURE_2D);
    glDepthFunc(GL_LEQUAL);   // Type of depth test to do

    //glCullFace(GL_BACK);
    //glFrontFace(GL_CCW);
    //glEnable(GL_CULL_FACE);

    // Use perspective correct interpolation if available
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    // Initialize the matrix stacks
    reshape(width, height);

    // Define lighting for the scene
    float lightDirection[]   = {1.0, 1.0, 1.0, 0};
    float ambientIntensity[] = {0.1, 0.1, 0.1, 1.0};
    float lightIntensity[]   = {0.9, 0.9, 0.9, 1.0};
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientIntensity);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightIntensity);
    glLightfv(GL_LIGHT0, GL_POSITION, lightDirection);
    glEnable(GL_LIGHT0);

    { // Initialize sphere
        createSphere(Radius,0,0,0);
    }

    { // Initialize eye transform
        for(int i = 0; i< eyeCount; i++)
        {
            double* eye = eyes[i];
            wld2eye.push_back(matrix4());
            glPushMatrix();
            glLoadIdentity();
            gluLookAt(eye[0], eye[1], eye[2], eye[3], eye[4], eye[5], eye[6], eye[7], eye[8]);
            wld2eye[i].getCurrentOpenGLmatrix(GL_MODELVIEW_MATRIX);
            glPopMatrix();
            matrix4 invmat;
            invmat.inverse(wld2eye[i]);
            eye2wld.push_back(invmat);
        }
    }
    eyeIndex = 0;
}

static void
write_targa(const char *filename, const GLfloat *buffer, int width, int height)
{
    FILE *f = fopen(filename, "w");
    if(f) {
        int i,x,y;
        const GLfloat *ptr = buffer;
        cout << "osdemo, writing tga file" << endl;
        fputc (0x00, f);	/* ID Length, 0 => No ID	*/
        fputc (0x00, f);	/* Color Map Type, 0 => No color map included	*/
        fputc (0x02, f);	/* Image Type, 2 => Uncompressed, True-color Image */
        fputc (0x00, f);	/* Next five bytes are about the color map entries */
        fputc (0x00, f);	/* 2 bytes Index, 2 bytes length, 1 byte size */
        fputc (0x00, f);
        fputc (0x00, f);
        fputc (0x00, f);
        fputc (0x00, f);	/* X-origin of Image	*/
        fputc (0x00, f);
        fputc (0x00, f);	/* Y-origin of Image	*/
        fputc (0x00, f);
        fputc (width & 0xff, f);      /* Image Width	*/
        fputc ((width>>8) & 0xff, f);
        fputc (height & 0xff, f);     /* Image Height	*/
        fputc ((height>>8) & 0xff, f);
        fputc (0x18, f);		/* Pixel Depth, 0x18 => 24 Bits	*/
        fputc (0x20, f);		/* Image Descriptor	*/
        fclose(f);
        f = fopen( filename, "ab" );  /* reopen in binary append mode */
        for (y=height-1; y>=0; y--) {
            for (x=0; x<width; x++) {
                int r, g, b;
                i = (y*width + x) * 4;
                r = (int) (ptr[i+0] * 255.0);
                g = (int) (ptr[i+1] * 255.0);
                b = (int) (ptr[i+2] * 255.0);
                if (r > 255) r = 255;
                if (g > 255) g = 255;
                if (b > 255) b = 255;
                fputc(b, f); /* write blue */
                fputc(g, f); /* write green */
                fputc(r, f); /* write red */
            }
        }
    }
}

vector3 getDirVector(vector3){
    return vector3(0,0,0);
}

vector3 getUpVector(vector3){
    return vector3(0,0,0);
}

int main(int argc, char* argv[])
{
    /*
    glfwWindowHint(GLFW_SAMPLES, 8);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_OSMESA);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    */

    if(argc != 5){
        cout << "ERROR: Number of argument, Should be 5" << endl;
        return -1;
    }
    GLfloat * buffer;
    image_dir = argv[1];
    player = new Player(image_dir);

    width = 640;
    height = 360;
    int BPP = 32;
    glfwInit(); // Init OpenGL

    double R = 100.;
    string lon_str, lat_str;
    lon_str = argv[2];
    lat_str = argv[3];
    double lon, lat;
    lon = (double)atof(lon_str.c_str());
    lat = (double)atof(lat_str.c_str());
    double dir_x, dir_y, dir_z;
    dir_x = R * cos(lat) * cos(lon);
    dir_y = R * cos(lat) * sin(lon);
    dir_z = R * sin(lat);
    vector3 dirvector = vector3(dir_x, dir_y, dir_z);
    vector3 upvector;
    upvector.cross(dirvector, vector3(0., 1., 0.));
    double up_x, up_y, up_z;
    up_x = upvector.x;
    up_y = upvector.y;
    up_z = upvector.z;
    cout << "Lon, Lat : " + lon_str + " " + lat_str << endl;
    printf("Dir vector : %f %f %f\n", dir_x, dir_y, dir_z);
    printf("Up vector : %f %f %f\n", up_x, up_y, up_z);

    //pixels = (unsigned char *)malloc((int)(3 * width * height));

    // Show original image
    // namedWindow("Sample Video", 1);

#if OSMESA_MAJOR_VERSION * 100 + OSMESA_MINOR_VERSION >= 305
    OSMesaContext ctx = OSMesaCreateContextExt(GL_RGBA, 16, 0, 0, NULL);
#else
    OSMesaContext ctx = OSMesaCreateContext(GL_RGBA, NULL);
#endif
    if(!ctx){
        cout << "OSMesaCreateContext failed!" << endl;
        return 0;
    }

    buffer = (GLfloat *)malloc(width * height * 4 * sizeof(GLfloat));
    if(!buffer) {
        cout << "Alloc image buffer failed!" << endl;
        return 0;
    }

    if (!OSMesaMakeCurrent(ctx, buffer, GL_FLOAT, width, height)) {
        cout << "OSMesaMakeCurrent failed!" << endl;
        return 0;
    }
    initialize();
    display();

/*
#if GLFW_VERSION_MAJOR==3
    GLFWwindow* window = glfwCreateWindow(width, height, "", NULL, NULL);
    g_window = window;
    if(!window) {glfwTerminate(); return 1;}
    glfwMakeContextCurrent(window);

    initialize(); // Initialize sphere, eye matrices

    glfwSetKeyCallback(window, onKeyPress);
    glfwSetMouseButtonCallback(window, onMouseButton);
    glfwSetCursorPosCallback(window, onMouseDrag);

    while (!glfwWindowShouldClose(window))
    {
        display();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
#else
    // Create i window (8-bit depth-buffer, no alpha and stencil buffers, windowed)
    glfwOpenWindow(width, height, BPP/4, BPP/4, BPP/4, 1, 8, 1, GLFW_WINDOW) ;
    glfwSetWindowTitle("360 Player");				// Make window whose name is "Simple Scene".
    int rv,gv,bv;
    glGetIntegerv(GL_RED_BITS,&rv);					// Get the depth of red bits from GL.
    glGetIntegerv(GL_GREEN_BITS,&gv);				// Get the depth of green bits from GL.
    glGetIntegerv(GL_BLUE_BITS,&bv);				// Get the depth of blue bits from GL.
    printf( "Pixel depth = %d : %d : %d\n", rv, gv, bv );
    initialize();									// Initialize the other thing.

    glfwSetKeyCallback(onKeyPress);					// Register onKeyPress function to call that when user presses the keyboard.
    glfwSetMouseButtonCallback(onMouseButton);		// Register onMouseButton function to call that when user moves mouse.
    glfwSetMousePosCallback(onMouseDrag);			// Register onMouseDrag function to call that when user drags mouse.

    while(glfwGetWindowParam(GLFW_OPENED) )
    {
        display();
        glfwSwapBuffers();
    }
#endif
*/ // Not using GLFW in server, Use OSMesa Off-screen rendering

#ifdef SAVE_TARGA
    write_targa(argv[4], buffer, width, height);
#else
    write_ppm(argv[4], buffer, width, height);
#endif

    //glfwTerminate();
    OSMesaDestroyContext(ctx);
    return 0;
}
