#include "StdAfx.h"
#include "AppError.h"
CString AppError::m_csErrorMessage = L"No Error";

AppError::AppError(void)
{
}

AppError::~AppError(void)
{
}

void AppError::SetError( CString& csError_i )
{

    AppError::m_csErrorMessage = csError_i;
}
bool AppError::GetError( CString& csError_o )
{
    csError_o = AppError::m_csErrorMessage;
    return csError_o.Compare(L"No Error");
}
