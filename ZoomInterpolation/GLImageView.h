#pragma once

#include "GL\GL.h"

class GLSLShader;
class GLVertexBuffer;
class GLTexture;
class GLText;

/*
 This class prepares Zoomed image with the provided parameters.
 This class require different resources( Such as GLSLShader, GLTexture, GLVertexBuffert etc )
 to create a final image.
 This class is prepared to create Multiple views in ZoomInterpolation application.
 */
class GLImageView
{
public:
    GLImageView(void);
    ~GLImageView(void);

    void SetViewport( const int nLeft_i, const int nTop_i,
                      const int nWidth_i, const int nHeight_i );
    void SetVertexBuffer( GLVertexBuffer* pVertexBuffer_i );
    // Input flag indicate shader object need to delete.
    // All In View prepares different shader objects and put it in ImageView list.
    // When ImageView delete, GLSLShader objects deleted from this class.
    void SetShader( GLSLShader* pShader_i , const bool bDeleteShader_i = false );
    void SetText( GLText* pText_i );
    void SetTextureParam( GLTexture* pTexture_i, const int nTextureWidth_i,
                          const int nTextureHeight_i, const GLenum eTextureFilter_i );
    void Draw();

private:

    int m_nViewportLeft;
    int m_nViewportTop;
    int m_nViewportWidth;
    int m_nViewportHeight;
    int m_nTextureWidth;
    int m_nTextureHeight;
    GLTexture* m_pTexture;
    GLSLShader* m_pShader;
    GLVertexBuffer* m_pVertexBuffer;
    GLText*          m_pText;
    GLenum    m_eTextureFilter;
    bool m_bDeleteShader;
};
