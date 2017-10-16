#pragma once
#include "GL\gl.h"
#include "Gl\glu.h"
// This class setup opengl. Craetes opengl rendering context and make it current.
class GLSetup
{
public:
	GLSetup(void);
	~GLSetup(void);
	bool InitGL( HWND pRenderWindow_i );
    bool CheckExtension( const CHAR* pExtensionToCheck_i );
	void Exit();
	void Draw();

private:
	HGLRC m_hRenderContext;
	HDC   m_hDeviceContext;
	HWND  m_hRenderWindow;
};
