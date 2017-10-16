#include "StdAfx.h"
#include "GLText.h"
#include "FontEngine.h"
#include "GL\gl.h"

GLText::GLText( FontEngine* pFontEngine_i )
{
    m_pFontEngine = pFontEngine_i;
    m_csDisplayString = "";
}

GLText::~GLText(void)
{
}

void GLText::SetText(const CString csNewText_i)
{
    m_csDisplayString = csNewText_i;
}

void GLText::Draw()
{
    m_pFontEngine->DrawText( m_csDisplayString );
}