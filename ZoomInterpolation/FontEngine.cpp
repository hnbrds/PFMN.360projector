#include "StdAfx.h"
#include "FontEngine.h"
#include "GL\gl.h"

FontEngine::FontEngine(void)
{
}

FontEngine::~FontEngine(void)
{
    Destroy();
}

bool FontEngine::Create( HDC hDeviceContext_i )
{
    // Creating font
    CFont *m_pFont;
    m_pFont = new CFont();

    VERIFY(m_pFont->CreateFont(
        15, // nHeight
        7, // nWidth
        0, // nEscapement
        0, // nOrientation
        FW_BOLD, // nWeight
        FALSE, // bItalic
        FALSE, // bUnderline
        0, // cStrikeOut
        ANSI_CHARSET, // nCharSet
        OUT_DEFAULT_PRECIS, // nOutPrecision
        CLIP_DEFAULT_PRECIS, // nClipPrecision
        ANTIALIASED_QUALITY, // nQuality
        DEFAULT_PITCH, // nPitchAndFamily
        L"Arial")); // lpszFacename

    HGDIOBJ hOldFont = ::SelectObject(hDeviceContext_i, m_pFont->m_hObject);

    if( !wglUseFontBitmaps( hDeviceContext_i, 0, 256 * 2, 1000 ))
    {
        return false;
    }

    ::SelectObject( hDeviceContext_i, hOldFont );

    delete m_pFont;
    return true;
}
void FontEngine::Destroy()
{
    glDeleteLists( 1000, 256 );
}

bool FontEngine::DrawText( LPCTSTR ptcString )
{
    glRasterPos2d( -0.5, -1.0 + 10.0 / 600.0 );
    glListBase( 1000 );
    glCallLists( wcslen( ptcString ), GL_UNSIGNED_SHORT, ptcString );
    return true;
}
