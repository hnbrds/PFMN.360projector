#include "StdAfx.h"
#include "GLVertexBuffer.h"


GLVertexBuffer::GLVertexBuffer(void)
{
    memset( m_pVertexBuffer, 0, sizeof(m_pVertexBuffer) );
}

// Here change vertex information of a particular index.
bool GLVertexBuffer::SetAt( int nIndex_i, float fX_i, float fY_i, float fZ_i, float fTu_i, float fTv_i )
{
	if( nIndex_i >= 4  )
	{
		return false;
	}
	m_pVertexBuffer[nIndex_i].x = fX_i;
	m_pVertexBuffer[nIndex_i].y = fY_i;
	m_pVertexBuffer[nIndex_i].z = fZ_i;
	m_pVertexBuffer[nIndex_i].tu = fTu_i;
	m_pVertexBuffer[nIndex_i].tv = fTv_i;
	return true;
}

void GLVertexBuffer::DrawVertexBuffer( GLenum eDrawPrimitive_i )
{
    glBegin( eDrawPrimitive_i );
    for( int nIndex = 0; nIndex < 4; nIndex++ )
    {
        GLVertex& Temp = m_pVertexBuffer[nIndex];
        glTexCoord2f( Temp.tu, Temp.tv );
        glVertex2f( Temp.x, Temp.y);
    }
    glEnd();
}


void GLVertexBuffer::CreateQuadVertexBuffer()
{
    SetAt( 0, -1.0f,1.0f, 0.0f,    0.0f, 1.0f); // Left Top  corner
    SetAt( 1, -1.0f,-1.0f, 0.0f,   0.0f, 0.0f), // Left Bottom
    SetAt( 2, 1.0f , -1.0f, 0.0f,  1.0f, 0.0f); // Right bottom
    SetAt( 3, 1.0f, 1.0f,  0.0f,   1.0f, 1.0f); // Right top
}