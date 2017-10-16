#include "StdAfx.h"
#include "GLTexture.h"
#include "GLExtension.h"
#include "GL\Glu.h"
#include "math.h"

GLTexture::GLTexture(void)
{
	m_nTextureID = 0;
}

GLTexture::~GLTexture(void)
{
	Delete();
}
bool GLTexture::Create(int nWidth, int nHeight, void *pbyData, int nFormat_i, int nInternalFormat_i)
{
    int nError = glGetError();
	if( 0 != m_nTextureID )
	{
		// if this texture already exists then delete it.
		Delete();
	}
	if( 0 == pbyData )
	{
		return false;
	}
	glEnable( GL_TEXTURE_2D );
	glGenTextures( 1, &m_nTextureID );

	glBindTexture( GL_TEXTURE_2D, m_nTextureID );

	glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );

    glTexImage2D( GL_TEXTURE_2D, 0, nInternalFormat_i, nWidth, nHeight, 0,
	    nFormat_i, GL_UNSIGNED_BYTE, pbyData );
    return ( GL_NO_ERROR == glGetError() );
}

void GLTexture::SetFilterType( const GLint FilterType_i, const GLint FilterValue_i )
{
    glBindTexture( GL_TEXTURE_2D, m_nTextureID );

    glTexParameteri( GL_TEXTURE_2D, FilterType_i, FilterValue_i );
}

bool GLTexture::GetData( BYTE*& pbyData )
{
    if( m_nTextureID )
    {
        glBindTexture( GL_TEXTURE_2D, m_nTextureID );
        glGetTexImage( GL_TEXTURE_2D, 0,GL_BGR_EXT, GL_UNSIGNED_BYTE, pbyData );
        return true;
    }
    return false;
}
void GLTexture::Delete()
{
	if( 0 != m_nTextureID )
	{
		glDeleteTextures( 1, &m_nTextureID );
		m_nTextureID = 0;
	}
}

void GLTexture::Enable()
{
    glEnable( GL_TEXTURE_2D );
    glBindTexture( GL_TEXTURE_2D, m_nTextureID );
}