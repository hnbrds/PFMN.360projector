#pragma once

class AppError
{
public:
    static void SetError( CString& csError_i );
    static bool GetError( CString& csError_o );
private:
    AppError(void);
    ~AppError(void);
private:
    static CString m_csErrorMessage;
};
