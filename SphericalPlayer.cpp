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
#include <string>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#elif __LINUX
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>

#include <assert.h>
//#include <direct.h>
#include <sys/stat.h>

#include "math/matrix4.h"
#include "math/vector3.h"
#include "cv/VideoFrame.h"

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
int eyeCount = sizeof(eyes) / sizeof(eyes[0]);
int eyeIndex;

double eyes_lon[9] = {0, 40, 80, 120, 160, 200, 240, 280, 320};
double eyes_lat[9] = {-75, -55, -35, -15, 0, 15, 35, 55, 75};

int eyeLonCount = sizeof(eyes_lon) / sizeof(eyes_lon[0]);
int eyeLatCount = sizeof(eyes_lat) / sizeof(eyes_lat[0]);
int eyeLonIndex, eyeLatIndex;

// Camera vectors
double up_x, up_y, up_z;
double dir_x, dir_y, dir_z;

float angle = 0.0f, ratio;
float x, y, z;
float lx, ly, lz;


//vector<matrix4> wld2eye, eye2wld;
matrix4 wld2eye, eye2wld;

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

int zAxisRotDeg, xAxisRotDeg, yAxisRotDeg;


// Width and Height for video player window
int width, height;
static int isDrag = 0;

string image_dir;
// Screen pixels
Mat frame;
GLfloat* pixels;

bool start = false;


/*
 Setting Camera vector for gluLookAt
 */
vector3 getDirVector(double lon, double lat)
{
    double R = 100.;
    double x, y, z;
    x = R * cos(lat) * cos(lon);
    z = R * cos(lat) * sin(lon);
    y = R * sin(lat);
    printf("Dir vector: %f %f %f\n", x, y, z);
    return vector3(x, y, z);
}


vector3 getUpVector(vector3 dirvec)
{
    /*
     vector3 tmpvec, upvec;
     tmpvec.cross(dirvec, XAXIS);
     upvec.cross(tmpvec, dirvec);
     return upvec;
     */
    if(dirvec.x == 0. && dirvec.y == 0.){
        //if(dirvec.z == 1. || dirvec.z == -1.)
        return ZAXIS;
    }
    else{
        return YAXIS;
    }
}


void setCamera(double lon, double lat){
    vector3 dirvec = getDirVector(lon, lat);
    vector3 upvec = getUpVector(dirvec);
    dir_x = dirvec.x;
    dir_y = dirvec.y;
    dir_z = dirvec.z;
    up_x = upvec.x;
    up_y = upvec.y;
    up_z = upvec.z;
}

typedef struct stat Stat;
/* Make all the directories leading up to PATH, then create PATH.  Note that
 this changes the process's umask; make sure that all paths leading to a
 return reset it to ORIGINAL_UMASK */

static int do_mkdir(const char *path, mode_t mode)
{
    Stat            st;
    int             status = 0;
    
    if (stat(path, &st) != 0)
    {
        /* Directory does not exist. EEXIST for race condition */
        if (mkdir(path, mode) != 0 && errno != EEXIST)
            status = -1;
    }
    else if (!S_ISDIR(st.st_mode))
    {
        errno = ENOTDIR;
        status = -1;
    }
    
    return(status);
}

/**
 ** mkpath - ensure all directories in path exist
 ** Algorithm takes the pessimistic view and works top-down to ensure
 ** each directory in path exists, rather than optimistically creating
 ** the last element and working backwards.
 */
int mkpath(const char *path, mode_t mode)
{
    char           *pp;
    char           *sp;
    int             status;
    char           *copypath = strdup(path);
    
    status = 0;
    pp = copypath;
    while (status == 0 && (sp = strchr(pp, '/')) != 0)
    {
        if (sp != pp)
        {
            /* Neither root nor double slash in path */
            *sp = '\0';
            status = do_mkdir(copypath, mode);
            *sp = '/';
        }
        pp = sp + 1;
    }
    if (status == 0)
        status = do_mkdir(path, mode);
    free(copypath);
    return (status);
}



static void
write_targa(const char *filename, const GLfloat *buffer, int width, int height)
{
    FILE *f = fopen( filename, "w" );
    if (f) {
        int i, x, y;
        const GLfloat *ptr = buffer;
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
                i = (y * width + x) * 4;
                r = (int) (ptr[i+0] * 255.0);
                g = (int) (ptr[i+1] * 255.0);
                b = (int) (ptr[i+2] * 255.0);
                //printf("R G B : %d %d %d\n", r, g, b);
                if (r > 255) r = 255;
                if (g > 255) g = 255;
                if (b > 255) b = 255;
                fputc(b, f); /* write blue */
                fputc(g, f); /* write green */
                fputc(r, f); /* write red */
            }
        }
        fclose(f);
    }
    glFlush();
}


void save_tga(){
    glReadPixels(0, 0, width, height, GL_RGBA, GL_FLOAT, pixels);
    string name = player->filename + "_" + to_string((int)eyes_lon[eyeLonIndex]) + "_" + to_string((int)eyes_lat[eyeLatIndex]) + ".tga";
    
    if(mkdir((player -> fileroot + "Projection").c_str(), S_IRUSR | S_IWUSR | S_IXUSR) == 0)
        cout << "Create dir : " + player -> fileroot + "Projection";
    
    //printf("mkdir result : %d\n", result);
    
    write_targa((player -> fileroot + "Projection/" + name).c_str(), pixels, width, height);
}

void save_jpg(){
    glReadPixels(0, 0, width, height, GL_RGBA, GL_FLOAT, pixels);
    string name = player->filename + "_" + to_string((int)eyes_lon[eyeLonIndex]) + "_" + to_string((int)eyes_lat[eyeLatIndex]) + ".jpg";
    
    cout <<"/Volumes/JYS/Projection/" + player -> fileroot << endl;
    mkpath(("/Volumes/JYS/Projection/" + player -> fileroot).c_str(), 0755);
    
    player -> outputFrame(("/Volumes/JYS/Projection/" + player -> fileroot + name).c_str(), pixels, width, height);
}

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
    glRotatef (90, 0, 0, 1);
    
    //eyeLonIndex = eyeIndex / eyeLatCount;
    //eyeLatIndex = eyeIndex % eyeLatCount;
    
    // Rotate by x-axis : Latitude
    glRotatef ((int)eyes_lat[eyeLatIndex] + (float)xAxisRotDeg, -1.0, 0.0, 0.0);
    glRotatef ((float)yAxisRotDeg, 0.0, -1.0, 0.0);
    // Rotate by z-axis : Longitude
    glRotatef ((int)eyes_lon[eyeLonIndex] + (float)zAxisRotDeg, 0.0, 0.0, -1.0);
    
    //glRotatef (30., 1.0, 0.0, 0.0);
    //glRotatef (329., 0.0, 1.0, 0.0);
    //glRotatef (75., 0.0, 0.0, 1.0);
    
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
    frame = player -> getCurrentFrame();
    imshow("Original Image", frame);

    // Call OpenCV to get next frame
    sphereText = MatToTexture(frame);

    // call display() for every frame of the video
    glClearColor(0., 0.25f, 1.0f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // set view transformation
    glLoadMatrixd(wld2eye.GLmatrix());

    ///glMatrixMode(GL_MODELVIEW);
    ///glLoadIdentity();
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, sphereText);
    
    drawSphere(8, sphereText);
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
    gluPerspective(60.0f, aspect,  1, 1024);
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


void nextProj(void) {
    if(eyeIndex == eyeLonCount * eyeLatCount - 1){
        frame = player -> getNextFrame();
        eyeIndex = 0;
    }
    else
        eyeIndex = (eyeIndex + 1) % (eyeLonCount * eyeLatCount);
    
    eyeLonIndex = eyeIndex / eyeLatCount;
    eyeLatIndex = eyeIndex % eyeLatCount;
    
    printf("Lon Lat : %d %d\n", (int)eyes_lon[eyeLonIndex], (int)eyes_lat[eyeLatIndex]);
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
        cout << "Start projection" << endl;
        start = true;
    }
    else if (key == ']') {
        frame = player -> getNextFrame();
    }
    else if (key == GLFW_KEY_N) {
        nextProj();
        glFlush();
    }
    else if (key == GLFW_KEY_S) {
        save_tga();
    }
    else if (key == GLFW_KEY_RIGHT) {
        zAxisRotDeg = (zAxisRotDeg + 1) % 360;
        printf("Rotation degree X, Y, Z : %d %d %d\n", xAxisRotDeg, yAxisRotDeg, zAxisRotDeg);
    }
    else if (key == GLFW_KEY_LEFT) {
        zAxisRotDeg = (zAxisRotDeg + 359) % 360;
        printf("Rotation degree X, Y, Z : %d %d %d\n", xAxisRotDeg, yAxisRotDeg, zAxisRotDeg);
    }
    else if (key == GLFW_KEY_UP) {
        xAxisRotDeg = (xAxisRotDeg + 1) % 360;
        printf("Rotation degree X, Y, Z : %d %d %d\n", xAxisRotDeg, yAxisRotDeg, zAxisRotDeg);
    }
    else if (key == GLFW_KEY_DOWN) {
        xAxisRotDeg = (xAxisRotDeg + 359) % 360;
        printf("Rotation degree X, Y, Z : %d %d %d\n", xAxisRotDeg, yAxisRotDeg, zAxisRotDeg);
    }
    else if (key == GLFW_KEY_Z){
        yAxisRotDeg = (yAxisRotDeg + 1) % 360;
        printf("Rotation degree X, Y, Z : %d %d %d\n", xAxisRotDeg, yAxisRotDeg, zAxisRotDeg);
    }
    else if (key == GLFW_KEY_X){
        yAxisRotDeg = (yAxisRotDeg + 359) % 360;
        printf("Rotation degree X, Y, Z : %d %d %d\n", xAxisRotDeg, yAxisRotDeg, zAxisRotDeg);
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
    
    {
        /*
        for(int i = 0; i < eyeLonCount; i++){
            for(int j = 0; j < eyeLatCount; j++){
                int idx = i * eyeLonCount + j;
                
                double* eye = eyes[idx];
                wld2eye.push_back(matrix4());
                glPushMatrix();
                glLoadIdentity();
                setCamera(eyes_lon[i], eyes_lat[j]);
                /*
                 * gluLookAt
                 * eyex, eyey, eyez : camera position
                 * centerx, centery, centerz : point camera is looking at
                 * upx, upy, upz : camera upward vector
                 
                //gluLookAt(0., 0., 0., dir_x, dir_y, dir_z, up_x, up_y, up_z);
                wld2eye[idx].getCurrentOpenGLmatrix(GL_MODELVIEW_MATRIX);
                glPopMatrix();
                matrix4 invmat;
                
                invmat.inverse(wld2eye[idx]);
                eye2wld.push_back(invmat);
            }
        }*/
        
        gluLookAt(0., 0., 0., 1., 0., 0., 0., 1., 0.);
        wld2eye.getCurrentOpenGLmatrix(GL_MODELVIEW_MATRIX);
        glPopMatrix();
        matrix4 invmat;
        invmat.inverse(wld2eye);
        eye2wld = invmat;
    }
    eyeIndex = 0;
}

int main(int argc, char* argv[])
{
    
    glfwWindowHint(GLFW_SAMPLES, 8);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    /*if(argc != 2){
        cout << "ERROR: Number of argument, Should be 2" << endl;
        return -1;
    }*/
    //image_dir = argv[1];
    //player = new Player(image_dir);
    
    
    player = new Player();
    
    width = 360;
    height = 200;
    int BPP = 32;
    
    glfwInit(); // Init OpenGL

    pixels = (GLfloat *)malloc(sizeof(GLfloat) * (4 * width * height));

    // Show original image
    namedWindow("Original Image", CV_WINDOW_AUTOSIZE);

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
        if(start == true){
            //save_tga();
            save_jpg();
            nextProj();
        }
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

    glfwTerminate();
    return 0;
}
