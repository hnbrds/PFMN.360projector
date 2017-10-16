#pragma once

/*
Creating a font display list, and draw all text.
All text drawing is handled with this class.
This class hold width and height of all characters to draw string in
correct alignment and position.
*/
class FontEngine
{
public:
    FontEngine(void);
    ~FontEngine(void);
    bool Create( HDC hDeviceContext_i );
    void Destroy();
    bool DrawText( LPCTSTR ptcString );
};
