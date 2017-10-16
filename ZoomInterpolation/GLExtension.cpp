#include "StdAfx.h"
#include "GLExtension.h"
#include "Gl\gl.h"

// Macro for getting function pointers of Extension functions.
#define GET_PROC_ADDRESS(fname, failedFunName)\
{\
    Obj_##fname = (FPTR_##fname)wglGetProcAddress( #fname ); \
    if( 0 == Obj_##fname ) \
    { \
        failedFunName = new TCHAR[strlen(#fname)+1]; \
        wcscpy( failedFunName, L#fname ); \
        return false; \
    } \
}\


// This initialization function should be called before calling any member functions in this class.
// Here create the function pointer for all extension functions.
// pFailedFunction Return NULL is all wglGetProcAddress are success.
// Else return the name of failed function. Application should delete this buffer.
bool GLExtension::GetWglProcAddress( TCHAR*& pFailedFunction )
{
    pFailedFunction = 0;
    // name of failed function
    GET_PROC_ADDRESS(glCreateProgramObjectARB,pFailedFunction);
    GET_PROC_ADDRESS( glCreateProgramObjectARB ,pFailedFunction);
    GET_PROC_ADDRESS( glCreateShaderObjectARB ,pFailedFunction); 
    GET_PROC_ADDRESS( glShaderSourceARB ,pFailedFunction); 
    GET_PROC_ADDRESS( glCompileShader ,pFailedFunction); 
    GET_PROC_ADDRESS( glAttachObjectARB ,pFailedFunction); 
    GET_PROC_ADDRESS( glLinkProgramARB ,pFailedFunction); 
    GET_PROC_ADDRESS( glUseProgramObjectARB ,pFailedFunction); 
    GET_PROC_ADDRESS( glDeleteObjectARB ,pFailedFunction); 
    GET_PROC_ADDRESS( glGetInfoLogARB ,pFailedFunction); 
    GET_PROC_ADDRESS( glGetUniformLocationARB ,pFailedFunction); 
    GET_PROC_ADDRESS( glGetAttribLocationARB ,pFailedFunction); 
    GET_PROC_ADDRESS( glUniform1iARB ,pFailedFunction); 
    GET_PROC_ADDRESS( glUniform1fARB ,pFailedFunction);
    GET_PROC_ADDRESS( glActiveTexture ,pFailedFunction);
    return true;
}
GLExtension::GLExtension(void)
{
    const char* pExensions = (const char* )glGetString( GL_EXTENSIONS );
    m_bNonPowerOfTwo = strstr( pExensions, "GL_ARB_texture_non_power_of_two" );
}

GLExtension::~GLExtension(void)
{
}

GLExtension& GLExtension::GetInstance()
{
    static GLExtension localInstance;
    return localInstance;
}
GLhandleARB GLExtension::glCreateProgramObjectARB()
{
    if( Obj_glCreateProgramObjectARB )
    {
        return(GLhandleARB)(*Obj_glCreateProgramObjectARB)();
    }
    return (GLhandleARB)0;
}
GLhandleARB GLExtension::glCreateShaderObjectARB(GLenum shaderType)
{
    if( Obj_glCreateShaderObjectARB )
    {
        GLhandleARB hHandle = (*Obj_glCreateShaderObjectARB)(shaderType);
        return hHandle;
    }
    return (GLhandleARB)0;
}
void GLExtension::glShaderSourceARB( GLhandleARB shader, GLuint number_strings,
                                     const GLcharARB** strings, GLint *length)
{
    if( Obj_glShaderSourceARB )
    {
        (*Obj_glShaderSourceARB)(shader, 1, strings, length);
    }
}

void GLExtension::glCompileShader( GLhandleARB shader )
{
    if( Obj_glCompileShader )
    {
        (*Obj_glCompileShader)( shader );
    }
}
void GLExtension::glAttachObjectARB(GLhandleARB program, GLhandleARB shader)
{
    if( Obj_glAttachObjectARB )
    {
        (*Obj_glAttachObjectARB)(program,shader);
    }
}
void GLExtension::glLinkProgramARB(GLhandleARB program)
{
    if( Obj_glLinkProgramARB )
    {
        (*Obj_glLinkProgramARB)(program);
    }
}
void GLExtension::glUseProgramObjectARB(GLhandleARB program)
{
    if( Obj_glUseProgramObjectARB )
    {
        (*Obj_glUseProgramObjectARB)(program);
    }
}
void GLExtension::glDeleteObjectARB(GLhandleARB object)
{
    if( Obj_glDeleteObjectARB )
    {
        (*Obj_glDeleteObjectARB)(object);
    }
}
void GLExtension::glGetInfoLogARB(GLhandleARB object, GLsizei maxLenght,
                                  GLsizei *length, GLbyte*infoLog)
{
    if(Obj_glGetInfoLogARB)
    {
        (*Obj_glGetInfoLogARB)(object,maxLenght, length, infoLog);
    }
}
GLint GLExtension::glGetUniformLocationARB(GLhandleARB program, const GLbyte* name)
{
    if( Obj_glGetUniformLocationARB )
    {
        return (*Obj_glGetUniformLocationARB)(program,name);
    }
    return 0;
}
GLint GLExtension::glGetAttribLocationARB(GLhandleARB program, const GLbyte* name)
{
    if( Obj_glGetAttribLocationARB )
    {
       return (*Obj_glGetAttribLocationARB)( program, name );
    }
    return 0;
}

void GLExtension::glUniform1iARB(GLuint index, int val)
{
    if( Obj_glUniform1iARB )
    {
        (*Obj_glUniform1iARB)( index, val );
    }
}

void GLExtension::glUniform1fARB(GLuint index, float fval)
{
    if( Obj_glUniform1fARB )
    {
        (*Obj_glUniform1fARB)( index, fval );
    }
}

void GLExtension::glActiveTexture(GLenum Texture)
{
    if( Obj_glActiveTexture )
    {
        (*Obj_glActiveTexture)( Texture );
    }
}
