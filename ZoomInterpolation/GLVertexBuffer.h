#pragma once

#include "GL\gl.h"

struct GLVertex
{
	// GL_T2F_V3F
	float tu, tv;
	float x, y, z;
};
// This class create a set of vertices's of GL_T2F_V3F type.
// SetAt will update the vertex information of a index.
// DrawVertexBuffer will pass the vertex informations to GPU.
class GLVertexBuffer
{
public:
	GLVertexBuffer(void);
    void CreateQuadVertexBuffer();
	bool SetAt( int nIndex_i, float fX_i, float fY_i, float fZ_i,
				float fTu_i, float fTv_i );
	void DrawVertexBuffer( GLenum eDrawPrimitive_i );
    GLVertex Get( int nIndex_i )
    {
        return m_pVertexBuffer[nIndex_i];
    }
    GLVertex    m_pVertexBuffer[4];
};
