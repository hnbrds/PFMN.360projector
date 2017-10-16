#include "StdAfx.h"
#include "GLSLShader.h"
#include "GL\gl.h"
#include "AppError.h"

GLSLShader::GLSLShader(void)
{

    m_hProgramObject = 0;
    m_hShaderHandle = 0;
}

GLSLShader::~GLSLShader(void)
{
    DeleteShader();
}

bool GLSLShader::CreateProgram( const int nProgramID_i, GLenum glSlShaderType_i )
{

    m_hProgramObject = GLExtension::GetInstance().glCreateProgramObjectARB();

    HRSRC hrResInfo = FindResource( 0, MAKEINTRESOURCE( nProgramID_i ),TEXT("GLSL_SHADER"));
    if( 0 == hrResInfo )
    {
        // Set Error messages.
        AppError::SetError( CString(L"FindResource returns NULL for shader ID.") );
        return false;
    }
    HGLOBAL hRcResource =  LoadResource(0, hrResInfo  );
    CHAR* pBufferResData = (CHAR* )LockResource( hRcResource );
    
    m_hShaderHandle = GLExtension::GetInstance().glCreateShaderObjectARB( GL_FRAGMENT_PROGRAM_ARB );
    if( 0 == m_hShaderHandle )
    {
        // Set Error messages.
        AppError::SetError( CString(L"0 == m_hShaderHandle GL_FRAGMENT_PROGRAM_ARB shader creation failed") );
        return false;
    }

    int nLENGTH = SizeofResource( 0, hrResInfo );
    TCHAR tcBuffer[333];
    swprintf( tcBuffer, L"LOG %d", nLENGTH );
    OutputDebugString(tcBuffer);
    BYTE* pbyShaderData = new BYTE[nLENGTH + 100];
    memcpy( pbyShaderData, pBufferResData, nLENGTH );
    pbyShaderData[nLENGTH] = 0;

    GLExtension::GetInstance().glShaderSourceARB( m_hShaderHandle, strlen( pBufferResData ), (const GLcharARB**)&pbyShaderData, &nLENGTH );
    GLExtension::GetInstance().glCompileShader( m_hShaderHandle );
    
    int nLen = 0; 
    char cBuffer[800];
    GLExtension::GetInstance().glGetInfoLogARB(m_hShaderHandle, 800,&nLen,(GLbyte*)cBuffer);
    if( nLen )
    {
        WCHAR *pWideChar = new WCHAR[nLen + 1];
        int nOutCount = 0;
        //MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED,cBuffer,nLen, pWideChar, nOutCount );
        for( int nStr =0; nStr < nLen; nStr++)
        {
            short s = cBuffer[nStr];
            pWideChar[nStr] = s;
        }
        pWideChar[nLen] = 0;
        // Set Error messages.
        AppError::SetError( CString( L"Shader Compilation failed.\n Error:\n" )+ CString( pWideChar ) );
        TRACE(  CString( L"Shader Compilation failed.\n Error:\n" )+ CString( pWideChar ) );
        delete[] pWideChar;
        // return false;
    }
    GLExtension::GetInstance().glAttachObjectARB( m_hProgramObject, m_hShaderHandle );
    delete[] pbyShaderData;
    return true;
}

bool GLSLShader::DeleteShader()
{
    if( m_hShaderHandle )
    {
        GLExtension::GetInstance().glDeleteObjectARB( m_hShaderHandle );
        m_hShaderHandle = 0;
    }
    if( GLSLShader::m_hProgramObject )
    {
        GLExtension::GetInstance().glDeleteObjectARB( GLSLShader::m_hProgramObject );
        GLSLShader::m_hProgramObject = 0;
    }
    return true;
}

bool GLSLShader::EnableShader()
{
    GLExtension::GetInstance().glLinkProgramARB( m_hProgramObject );
    return true;
}
bool GLSLShader::DisableShader()
{
    int nErr = 0;
    GLExtension::GetInstance().glUseProgramObjectARB( 0 );
    return true;
}

bool GLSLShader::SetTexture( const CHAR* pParamName_i, const int nTextureID_i )
{
    GLExtension::GetInstance().glUseProgramObjectARB( m_hProgramObject );
    GLint nParamObj = GLExtension::GetInstance().glGetUniformLocationARB( m_hProgramObject, (const GLbyte*)pParamName_i );
    if( -1 == nParamObj )
    {
        return false;
    }

    GLExtension::GetInstance().glActiveTexture( GL_TEXTURE0 + nTextureID_i );
    glBindTexture(GL_TEXTURE_2D, nTextureID_i);

    GLExtension::GetInstance().glUniform1iARB( nParamObj, nTextureID_i );

    GLExtension::GetInstance().glActiveTexture( GL_TEXTURE0 );
    glBindTexture(GL_TEXTURE_2D, nTextureID_i);

    return ( GL_NO_ERROR == glGetError() );
}
bool GLSLShader::SetParam( const CHAR* pParamName_i, const float fValue_i )
{
    GLExtension::GetInstance().glUseProgramObjectARB( m_hProgramObject );
    GLint nParamObj = GLExtension::GetInstance().glGetUniformLocationARB( m_hProgramObject, (const GLbyte*)pParamName_i );
    if( -1 == nParamObj )
    {
        return false;
    }

    GLExtension::GetInstance().glUniform1fARB( nParamObj, fValue_i );
    return ( GL_NO_ERROR == glGetError() );
}

