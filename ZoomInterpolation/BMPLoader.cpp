#include "StdAfx.h"
#include "BMPLoader.h"
#include "stdio.h"
#include "math.h"
#include "gdiplus.h"
#include "GdiplusImaging.h"
#include "Gdipluspixelformats.h"

using namespace Gdiplus;
BMPLoader::BMPLoader(void)
{
}

BMPLoader::~BMPLoader(void)
{
}

// This function read resource and output width,height and bitmap data.
bool BMPLoader::LoadBMP( const WCHAR* pFileName_i, int& nWidth_o, int& nHeight_o, BYTE*& pbyData_o )
{
    Gdiplus::Bitmap* m_pBitmap = new Gdiplus::Bitmap(pFileName_i, true);
    
    BYTE* pbyData = 0;
    int nWidth = m_pBitmap->GetWidth();
    int nHeight = m_pBitmap->GetHeight();
    Gdiplus::Rect rect(0,0,nWidth,nHeight);
    Gdiplus::BitmapData pBMPData;
    m_pBitmap->LockBits( &rect,Gdiplus::ImageLockMode::ImageLockModeRead , PixelFormat24bppRGB, &pBMPData );
    int nBytesInARow = ceil( nWidth * 3 / 4.0 ) * 4.0;
    pbyData_o = new BYTE[nBytesInARow * nHeight ];
    nWidth_o = nWidth;
    nHeight_o = nHeight;
    if( 0 == pBMPData.Scan0 )
    {
        return false;
    }
    BYTE* pSrc = (BYTE*)pBMPData.Scan0;
    int nVert = nHeight - 1;

    // When width * 3 is not divisible by 4, then need special handling.
    
    //memcpy( pbyData_o, pBMPData.Scan0, nWidth * nHeight * 3 );
    for( int nY = 0; nY < nHeight && nVert >= 0; nY++ )
    {
        BYTE* pDest = pbyData_o + ( nBytesInARow * nVert  );
        memcpy( pDest, pSrc, nBytesInARow );
        nVert--;
        pSrc += ( nBytesInARow );
    }
    m_pBitmap->UnlockBits( &pBMPData );
    delete m_pBitmap;
    return true;
}

// This function read resource and output width,height and bitmap data.
bool BMPLoader::LoadBMP( const int nResourceID_i, int& nWidth_o, int& nHeight_o, BYTE*& pbyData_o, 
                         bool bAlphaEnabled )
{
    try
    {
        int nNoOfChannels = bAlphaEnabled ? 4 : 3;
        pbyData_o = 0;
        nWidth_o = 0;
        nHeight_o = 0;
        HRSRC hrResInfo = FindResource( 0, MAKEINTRESOURCE( nResourceID_i ), RT_BITMAP );
        if( 0 == hrResInfo )
        {
            return false;
        }
        HGLOBAL hRcResource =  LoadResource(0, hrResInfo );
        CHAR* pBufferResData = (CHAR* )LockResource( hRcResource );
        if( 0 == pBufferResData )
        {
            return false;
        }
        // get BITMAPINFOHEADER
        BITMAPINFOHEADER bmpInfo;
        memcpy( &bmpInfo,pBufferResData, sizeof(BITMAPINFOHEADER) );
        pBufferResData += sizeof(BITMAPINFOHEADER);
        if( nNoOfChannels * 8 != bmpInfo.biBitCount )
        {
            AfxMessageBox( (LPCTSTR)L"Number of channels is not correct");
            return false;
        }
        nWidth_o = bmpInfo.biWidth;
        nHeight_o = bmpInfo.biHeight;
        int nBMPSize = nWidth_o * nHeight_o * nNoOfChannels;
        pbyData_o = new BYTE[nBMPSize];
        if( 0 == pbyData_o )
        {
            return false;
        }
        memcpy( pbyData_o, pBufferResData, nBMPSize );
        return true;
    }
    catch (CMemoryException* e)
    {
    	
    }
    catch (CFileException* e)
    {
    }
    catch (CException* e)
    {
    }
    return false;
}

bool BMPLoader::SaveBMP( const WCHAR* pFileName_i, int nWidth_i, int nHeight_i, BYTE* pbyData_i )
{
    FILE* pFile = 0;
    try
    {
        if( 0 == pbyData_i )
        {
            return false;
        }
        int nBMPType = 19778; // BM
        int nOffsetHeader = sizeof( BITMAPINFOHEADER ) + sizeof( BITMAPFILEHEADER );
        int nFileSize = ( nHeight_i * nWidth_i ) + nOffsetHeader;

        // Fill the Bitmap File header.
        BITMAPFILEHEADER stBitmapFileHeader;
        stBitmapFileHeader.bfType = nBMPType;
        stBitmapFileHeader.bfOffBits = nOffsetHeader; 
        stBitmapFileHeader.bfReserved1 = 0;
        stBitmapFileHeader.bfSize = nFileSize;
        stBitmapFileHeader.bfReserved2 = 0;

        int nWrappedWidth = nWidth_i;
        // Fill the Bitmap Info header.
        BITMAPINFO stBitmapInfo;
        stBitmapInfo.bmiHeader.biClrImportant = 0;
        stBitmapInfo.bmiHeader.biClrUsed = 0;
        stBitmapInfo.bmiHeader.biCompression = 0;
        stBitmapInfo.bmiHeader.biPlanes = 1;
        stBitmapInfo.bmiHeader.biSize = sizeof( BITMAPINFOHEADER );
        stBitmapInfo.bmiHeader.biXPelsPerMeter = 0;
        stBitmapInfo.bmiHeader.biYPelsPerMeter = 0;
        stBitmapInfo.bmiHeader.biBitCount = 3 * 8;
        stBitmapInfo.bmiHeader.biSizeImage = nHeight_i * nWidth_i;
        stBitmapInfo.bmiHeader.biHeight = nHeight_i;
        stBitmapInfo.bmiHeader.biWidth = nWidth_i;
        stBitmapInfo.bmiColors[0].rgbBlue = 0x0;
        stBitmapInfo.bmiColors[0].rgbGreen = 0x0;
        stBitmapInfo.bmiColors[0].rgbRed = 0x0;
        stBitmapInfo.bmiColors[0].rgbReserved = 0x0;

        FILE* fpFile;
        _wfopen_s( &fpFile, pFileName_i, L"wb" );
        fwrite( &stBitmapFileHeader, 1, sizeof( BITMAPFILEHEADER ), fpFile);
        fwrite( &stBitmapInfo, 1, sizeof( BITMAPINFOHEADER ), fpFile);
        fwrite( pbyData_i, 1, ( nHeight_i * nWidth_i * 3 ), fpFile );
        fclose( fpFile );
        return true;
    }
    catch (CMemoryException* e)
    {
        if( pFile )
        {
            fclose( pFile );
        }
        return false;

    }
    catch (CFileException* e)
    {
        if( pFile )
        {
            fclose( pFile );
        }
        return false;
    }
    catch (CException* e)
    {
        if( pFile )
        {
            fclose( pFile );
        }
        return false;
    }
}