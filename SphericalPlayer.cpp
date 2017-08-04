/*********************************************************************************
 * Spherical Player for equirectangular video
 * GLFW_VERSION_MAJOR == 3
 **********************************************************************************/

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <math.h>

#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLFW/glfw3.h>
#include <assert.h>

#include "math/matrix4.h"
#include "WaveFrontOBJ.h"
#include "math/vector3.h"
#include "cv/VideoFrame.h"

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

#if GLFW_VERSION_MAJOR==3
GLFWwindow* g_window = NULL;
#endif

#define XAXIS vector3(1,0,0)
#define YAXIS vector3(0,1,0)
#define ZAXIS vector3(0,0,1)

using namepace std;

string video_dir;
Player* player;

double eye[1][9] =
{
    {28,18,18,0,2,0,0,1,0}
};
matrix4 wld2eye, eye2wld;
matrix4 sphere2wld;
WaveFrontOBJ* sphere;
int sphereID;
vector3 startPt;

// Width and Height for video player window
int width, height;
static int isDrag = 0;

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
 **********************************************************************************/
void drawSphere(matrix4 const& _sphere2wld)
{
    glPushMatrix();
    glMultMatrixd(_sphere2wld.GLmatrix());
    //drawFrame(5);
    glEnable(GL_LIGHTING);
    float frontColor[] = {1.0, 1.0, 1.0, 1.0};
    //glMaterialfv(GL_FRONT, GL_AMBIENT, frontColor);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, frontColor);
    
    glCallList(sphereID);
    glPopMatrix();
}


/*********************************************************************************
 * Call this part whenever display events are needed.
 * Display events are called in case of re-rendering by OS. ex) screen movement, screen maximization, etc.
 **********************************************************************************/
void display()
{
    // call display() for every frame of the video
    glClearColor(0., 0.25f, 1.0f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // set view transformation
    glLoadMatrixd(wld2eye.GLmatrix());
    
    drawSphere(sphere2wld);
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
    double aspect = width/double(height);
    gluPerspective(45, aspect, 1, 1024);
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

void RenderScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
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
}
    
/*********************************************************************************
* Load texure video from file
* (Equirectangular video)
**********************************************************************************/
unsigned int loadTexture(const char* filename)
{
    return 1;
}
    
void initialize()
{
    // Set up OpenGL state
    glShadeModel(GL_SMOOTH);  // Smooth shading
    glEnable(GL_DEPTH_TEST);  // Enable depth testing
    glDepthFunc(GL_LEQUAL);   // Type of depth test to do
    
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
        sphere = new WaveFrontOBJ("sphere.obj");
        
        sphereID = glGenLists(1);               // Create display lists
        glNewList(sphereID, GL_COMPILE);        // Begin compiliing the display list using sphereID
        
        sphere -> Draw();                       // Draw sphere on display list
        glEndList();                            // Terminate compiling the display list. Draw sphere using 'sphereID'
    
        glPushMatrix();                         // Push current matrix of GL into stack
        glLoadIdentity();                       // Set GL matrix Identity matrix
        // Read modelview matrix about location and direction set above, store it in sphere2wld matrix
        sphere2wld.getCurrentOpenGLmatrix(GL_MODELVIEW_MATRIX);
        glPopMatrix();
    }
    
    { // Initialize eye transform
        double* c = eye[0];
        glPushMatrix();
        glLoadIdentity();
        gluLookAt(c[0], c[1], c[2], c[3], c[4], c[5], c[6], c[7], c[8]);
        wld2eye.getCurrentOpenGLmatrix(GL_MODELVIEW_MATRIX);
        glPopMatrix();
        
        matrix4 invmat;
        invmat.inverse(wld2eye);
        eye2wld = invmat;
    }
}

int main(int argc, char* argv[])
{
    if(argc != 2){
        return -1;
    }
    video_dir = argv[1];
    
    width = 720;
    height = 720;
    int BPP = 32;
    glfwInit(); // Init OpenGL
    
#if GLFW_VERSION_MAJOR==3
    GLFWwindow* window = glfwCreateWindow(width, height, "360 Player", NULL, NULL);
    g_window=window;
    if(!window) {glfwTerminate(); return 1;}
    glfwMakeContextCurrent(window);
    
    initialize();									// Initialize sphere, eye matrices
    
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
    // Create a window (8-bit depth-buffer, no alpha and stencil buffers, windowed)
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
