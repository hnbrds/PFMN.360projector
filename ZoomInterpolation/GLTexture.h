#pragma once

#include "GL\gl.h"
// Handle texture related opertions in this class.
class GLTexture
{
public:
	GLTexture(void);
	~GLTexture(void);
	bool Create( int nWidth, int nHeight, void* pbyData, int nFormat_i = GL_BGR_EXT,
                 int nInternalFormat_i = GL_RGB8 );
    bool GetData( BYTE*& pbyData );
    void SetFilterType( const GLint FilterType_i, const GLint FilterValue_i );
	void Delete();
	void Enable();
	void Disable()
    {
        glDisable( GL_TEXTURE_2D );
    }
    int GetTextureID()
    {
        return m_nTextureID;
    }

private:

	GLuint m_nTextureID;
};
