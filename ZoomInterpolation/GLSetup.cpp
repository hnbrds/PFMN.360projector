#include "StdAfx.h"
#include "GLSetup.h"

GLSetup::GLSetup(void)
{
	m_hRenderContext = 0;
	m_hDeviceContext = 0;
	m_hRenderWindow = 0;
}

GLSetup::~GLSetup(void)
{
	Exit();
}

// Function for creating a rendering context and make it as current rendering context.
bool GLSetup::InitGL( HWND pRenderWindow_i )
{
	m_hRenderWindow = pRenderWindow_i;
	GLuint PixelFormat;

	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));

	pfd.nSize      = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion   = 1;
	pfd.dwFlags    = PFD_DRAW_TO_WINDOW |PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 16;
	pfd.cDepthBits = 16;

	m_hDeviceContext = ::GetDC( m_hRenderWindow );
	PixelFormat = ChoosePixelFormat( m_hDeviceContext, &pfd );
	SetPixelFormat( m_hDeviceContext, PixelFormat, &pfd);
	m_hRenderContext = wglCreateContext( m_hDeviceContext );
	wglMakeCurrent( m_hDeviceContext, m_hRenderContext );

    RECT Region;
    ::GetClientRect( m_hRenderWindow, &Region );

    //glMatrixMode(GL_PROJECTION);
    //glLoadIdentity();
//    int nWidth = (float)Region.right / 2;
//    int nHeight = (float)Region.bottom/ 2;
//    glOrtho( -nWidth, nWidth, -nHeight, nHeight, 0.0, 5.0 );

    //glMatrixMode( GL_MODELVIEW );
    //glLoadIdentity();
 
	// Check opengl error before exiting.
	return( GL_NO_ERROR == glGetError());
}

bool GLSetup::CheckExtension( const CHAR* pExtensionToCheck_i )
{
    const CHAR* pExtension = (const CHAR*)glGetString( GL_EXTENSIONS );
    if( 0 == pExtension )
    {
        return false;
    }
    if( 0 < strstr( pExtension, pExtensionToCheck_i ))
    {
        return true;
    }
    return false;
}
    

void GLSetup::Exit()
{
	if( m_hRenderContext != NULL )
	{
		wglMakeCurrent( NULL, NULL );
		wglDeleteContext( m_hRenderContext );
		m_hRenderContext = NULL;
	}

	if( m_hDeviceContext != NULL )
	{
		ReleaseDC( m_hRenderWindow, m_hDeviceContext );
		m_hDeviceContext = NULL;
	}
}

void GLSetup::Draw()
{
	SwapBuffers( m_hDeviceContext );
}
