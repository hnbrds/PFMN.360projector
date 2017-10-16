#pragma once

#include "GL\gl.h"
#include "GLExtension.h"

// Shader class handles creating of a Shader, and parameter setting to the 
// GLSL Shader. This class uses GLExtension for calling opengl extension functions.
class GLSLShader
{
public:
    GLSLShader(void);
    // nProgramID_i is the resource ID of Shader.
    // glSlShaderType_i should be GL_FRAGMENT_PROGRAM_ARB, GL_VERTEX_PROGRAM_ARB
    bool CreateProgram( const int nProgramID_i, GLenum glSlShaderType_i );
    bool DeleteShader();
    bool EnableShader();
    bool DisableShader();
    // This function attach a texture to shader parameter.
    // pParamName_i is parameter name given in shader.
    bool SetTexture( const CHAR* pParamName_i, const int nTextureID_i );
    // This function passes a float value to Shader float parameter.
    bool SetParam( const CHAR* pParamName_i, const float fValue_i );
    ~GLSLShader(void);

private:

    GLhandleARB m_hProgramObject;
    GLhandleARB m_hShaderHandle;
};
