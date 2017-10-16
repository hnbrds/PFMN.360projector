#pragma once

// This class can load a bitmap from a file or Resource.
// LoadBMP return the allocated buffer ,with width and height.
// GdiPlus functions are used to load different image file formats.
class BMPLoader
{
public:
    BMPLoader(void);
    ~BMPLoader(void);
    bool LoadBMP( const WCHAR* pFileName_i, int& nWidth_o, int& nHeight_o, BYTE*& pbyData_o );
    bool LoadBMP( const int nResourceID_i, int& nWidth_o, int& nHeight_o, BYTE*& pbyData_o,
                  bool bAlphaEnabled = false );
    bool SaveBMP( const WCHAR* pFileName_i, int nWidth_i, int nHeight_i, BYTE* pbyData_i );
};
