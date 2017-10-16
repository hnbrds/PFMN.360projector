#pragma once

#include "GLExtDef.h" // Hold definitions of opengl extensions.
#include "gl\gl.h"

typedef unsigned int GLhandleARB;
typedef char GLcharARB;

// This Singleton class hold all function pointers of opengl Extensions.
// Functions similar to opengl extension are provided in this class. 
// All function will call corresponding function pointer.
class GLExtension
{
private:
    // Typedefs.
    typedef GLhandleARB (APIENTRY*FPTR_glCreateProgramObjectARB)();
    typedef GLhandleARB (APIENTRY *FPTR_glCreateShaderObjectARB)( GLenum shaderType);
    typedef void (APIENTRY *FPTR_glShaderSourceARB)( GLhandleARB shader, GLuint number_strings,
                                                     const GLcharARB** strings, GLint* length);
    typedef void (APIENTRY* FPTR_glCompileShader)(GLhandleARB shader);
    typedef void (APIENTRY* FPTR_glAttachObjectARB)(GLhandleARB program, GLhandleARB shader);
    typedef void (APIENTRY* FPTR_glLinkProgramARB)(GLhandleARB program);
    typedef void (APIENTRY* FPTR_glUseProgramObjectARB)(GLhandleARB program);
    typedef void (APIENTRY* FPTR_glDeleteObjectARB)(GLhandleARB object);
    typedef void (APIENTRY* FPTR_glGetInfoLogARB)(GLhandleARB object, GLsizei maxLenght, GLsizei *length, GLbyte *infoLog);
    typedef GLint (APIENTRY* FPTR_glGetUniformLocationARB)(GLhandleARB program, const GLbyte * name);
    typedef GLint (APIENTRY* FPTR_glGetAttribLocationARB)(GLhandleARB program, const GLbyte* name);
    typedef void (APIENTRY* FPTR_glUniform1iARB)(GLuint index, int val);
    typedef void (APIENTRY* FPTR_glUniform1fARB)(GLuint index, float val);
    typedef void (APIENTRY * FPTR_glActiveTexture) (GLenum texture);

private:

    // Function pointers.
    FPTR_glCreateProgramObjectARB  Obj_glCreateProgramObjectARB;
    FPTR_glCreateShaderObjectARB   Obj_glCreateShaderObjectARB;
    FPTR_glShaderSourceARB         Obj_glShaderSourceARB;
    FPTR_glCompileShader           Obj_glCompileShader;
    FPTR_glAttachObjectARB         Obj_glAttachObjectARB;
    FPTR_glLinkProgramARB          Obj_glLinkProgramARB;
    FPTR_glUseProgramObjectARB     Obj_glUseProgramObjectARB;
    FPTR_glDeleteObjectARB         Obj_glDeleteObjectARB;
    FPTR_glGetInfoLogARB           Obj_glGetInfoLogARB;
    FPTR_glGetUniformLocationARB   Obj_glGetUniformLocationARB;
    FPTR_glGetAttribLocationARB    Obj_glGetAttribLocationARB;
    FPTR_glUniform1iARB            Obj_glUniform1iARB;
    FPTR_glUniform1fARB            Obj_glUniform1fARB;
    FPTR_glActiveTexture           Obj_glActiveTexture;

public:

    GLhandleARB glCreateProgramObjectARB();
    GLhandleARB glCreateShaderObjectARB(GLenum shaderType);
    void glShaderSourceARB( GLhandleARB shader, GLuint number_strings,
                            const GLcharARB** strings, GLint * length);
    void glAttachObjectARB(GLhandleARB program, GLhandleARB shader);
    void glLinkProgramARB(GLhandleARB program);
    void glCompileShader(GLhandleARB program);
    void glUseProgramObjectARB(GLhandleARB program);
    void glDeleteObjectARB(GLhandleARB object);
    void glGetInfoLogARB(GLhandleARB object, GLsizei maxLenght, GLsizei *length, GLbyte*infoLog);
    GLint glGetUniformLocationARB(GLhandleARB program, const GLbyte* name);
    GLint glGetAttribLocationARB(GLhandleARB program, const GLbyte* name);
    void glUniform1iARB(GLuint index, int val);
    void glUniform1fARB(GLuint index, float fval);
    void glActiveTexture(GLenum Texture);

public:

    static GLExtension& GetInstance();
    // pFailedFunction Return NULL is all wglGetProcAddress are success.
    // Else return the name of failed function. Application should delete this buffer.
    bool GetWglProcAddress( TCHAR*& pFailedFunction );

private:

    // Constructor and destructor defined in private section for Singleton behaviour.
    GLExtension(void);
    ~GLExtension(void);

public:

    bool m_bNonPowerOfTwo;
};
