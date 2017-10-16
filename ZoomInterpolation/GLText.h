
#pragma once

class FontEngine;

/*
This class simply handles drawing of Text.
*/
class GLText
{
public:
    GLText( FontEngine* pFontEngine_i );
    ~GLText();
    void SetText( const CString csNewText_i );
    void Draw();

    FontEngine* m_pFontEngine;
    CString     m_csDisplayString;
};
