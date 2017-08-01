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

// Width and Height for video player window
int w_width, w_height;
static int isDrag = 0;

/*********************************************************************************
 * Call this part whenever display events are needed.
 * Display events are called in case of re-rendering by OS. ex) screen movement, screen maximization, etc.
 **********************************************************************************/
void display()
{
    // call display() for every frame of the video

}


/*********************************************************************************
 * Call this part whenever size of the window is changed.
 **********************************************************************************/
void reshape(int w, int h)
{
    
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
        if(state = GLFW_DOWN)
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
        printf("Mouse Dragging\n")
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
    if (action==GLFW_RELEASE)
        return 	; // do nothing
    // If 'c' or space bar are pressed, alter the camera.
    // If a number is pressed, alter the camera corresponding the number.
    if ((key == ' ') || (key == 'c'))
    {
        printf( "Toggle camera %d\n", cameraIndex );
        cameraIndex += 1;
    }
    else if ((key >= '0') && (key <= '9'))
        cameraIndex = key - '0';
        
    if (cameraIndex >= (int)wld2cam.size() )
        cameraIndex = 0;
}
    
/*********************************************************************************
* Load texure video from file
* (Equirectangular video)
**********************************************************************************/
unsigned int loadTexture(const char* filename)
{
    
}
    
void initialize()
{
    // Set up OpenGL state
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
}

int main(int argc, char* argv[])
{
    width = 1280;
    height = 720;
    int BPP = 32;
    glfwInit();
    
#if GLFW_VERSION_MAJOR==3
    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Tutorialal", NULL, NULL);
    g_window=window;
    if(!window) {glfwTerminate(); return 1;}
    glfwMakeContextCurrent(window);
    
    initialize();									// Initialize the other thing.
    
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
    return 0;
}
