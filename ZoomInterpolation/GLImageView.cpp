#include "stdafx.h"
#include "GLImageView.h"
#include "GLVertexBuffer.h"
#include "GLSLShader.h"
#include "GLTexture.h"
#include "GLText.h"

GLImageView::GLImageView(void)
{
    // Initialize all members.
    m_nViewportHeight = 0;
    m_nViewportLeft = 0;
    m_nViewportWidth = 0;
    m_nViewportHeight = 0;
    m_pVertexBuffer = 0;
    m_pShader = 0;
    m_pTexture = 0;
    m_pText = 0;
    m_bDeleteShader = false;
}

GLImageView::~GLImageView(void)
{
    if( m_bDeleteShader )
    {
        delete m_pShader;
        m_pShader = 0;
    }
    if( m_pText )
    {
        delete m_pText;
        m_pText = 0;
    }
}

void GLImageView::SetViewport( const int nLeft_i, const int nTop_i,
                  const int nWidth_i, const int nHeight_i )
{
    m_nViewportLeft = nLeft_i;
    m_nViewportTop = nTop_i;
    m_nViewportWidth = nWidth_i;
    m_nViewportHeight = nHeight_i;
}
void GLImageView::SetVertexBuffer( GLVertexBuffer* pVertexBuffer_i )
{
    m_pVertexBuffer = pVertexBuffer_i;
}
void GLImageView::SetShader( GLSLShader* pShader_i, const bool bDeleteShader_i )
{
    m_pShader = pShader_i;
    m_bDeleteShader = bDeleteShader_i;
}
void GLImageView::SetText( GLText* pText_i )
{
    m_pText = pText_i;
}

void GLImageView::SetTextureParam( GLTexture* pTexture_i, const int nTextureWidth_i,
                                   const int nTextureHeight_i, const GLenum eTextureFilter_i )
{
    m_pTexture = pTexture_i;
    m_nTextureWidth = nTextureWidth_i;
    m_nTextureHeight = nTextureHeight_i;
    m_eTextureFilter = eTextureFilter_i;
}
void GLImageView::Draw()
{
    // Set viewport for Zoomed image display.
    glViewport( m_nViewportLeft, m_nViewportTop,
                m_nViewportWidth, m_nViewportHeight );

    glEnable( GL_TEXTURE_2D );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_eTextureFilter );
    if( 0 != m_pShader )
    {
        // If GLSL shader is selected use the same for interpolated image display.
        m_pShader->EnableShader();
        if( !m_pShader->SetTexture( "ImageTexture", m_pTexture->GetTextureID() ))
        {
            TRACE( L"SetTexture failed" );
        }

        if( !m_pShader->SetParam( "fWidth", m_nTextureWidth ))
        {
            TRACE( L"SetTexture failed" );
        }

        if( !m_pShader->SetParam( "fHeight", m_nTextureHeight ))
        {
            TRACE( L"SetTexture failed" );
        }
        m_pVertexBuffer->DrawVertexBuffer( GL_QUADS );
        m_pShader->DisableShader();
    }
    else
    {
        
        // OpenGL Interpolation.
        m_pTexture->Enable();
        m_pVertexBuffer->DrawVertexBuffer( GL_QUADS );
        m_pTexture->Disable();
    }

    glDisable( GL_TEXTURE_2D );
    glColor3f( 0.0, 0.0, 1.0 );

    glBegin( GL_LINE_LOOP );

    static float fPixel = 1.0 / 600.0;
    glVertex2d( -1+ fPixel, -1 + fPixel);
    glVertex3f( 1, -1 + fPixel, 0.0 );

    glVertex3f( 1, 1 , 0.0);
    glVertex3f( -1, 1 , 0.0);
    glVertex3f( -1 + fPixel, -1, 0.0 );
    glEnd();
    
    glDisable( GL_TEXTURE_2D );
    glColor3f( 1.0, 1.0, 1.0 );

    if( m_pText )
    {
        m_pText->Draw();
    }
}
