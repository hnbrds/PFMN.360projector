#include "stdafx.h"
#include "PlotInterpCurve.h"
#include "math.h"

PlotInterpCurve::PlotInterpCurve(void)
{
    m_hDrawWindow = 0;
    m_eInterpolationType = INTERP_LINEAR;
}

PlotInterpCurve::~PlotInterpCurve(void)
{
}

void PlotInterpCurve::SetInterpolationType( const INTERPOLATION_TYPE_e eInterpolationType_i )
{
    m_eInterpolationType = eInterpolationType_i;
}
void PlotInterpCurve::SetDrawWindow( const HWND hWindow_i )
{
    m_hDrawWindow = hWindow_i;
}

float PlotInterpCurve::BellFunc( float x )
{
    float f = ( x / 2.0 ) * 1.5;
    if( f > -1.5 && f < -0.5 )
    {
        return( 0.5 * pow(f + 1.5, 2.0));
    }
    else if( f > -0.5 && f < 0.5 )
    {
        return 3.0 / 4.0 - ( f * f );
    }
    else if( ( f > 0.5 && f < 1.5 ) )
    {
        return( 0.5 * pow(f - 1.5, 2.0));
    }
    return 0.0;
}

float PlotInterpCurve::Triangular( float f )
{
    f = f / 2.0;
    if( f < 0.0 )
    {
        return ( f + 1.0 );
    }
    else
    {
        return ( 1.0 - f  );
    }
    return 0.0;
}

float PlotInterpCurve::BSpline( float x )
{
    const float B = 1.0;
    const float C = 0.0;
    float f = x;
    if( f < 0.0 )
    {
        f = -f;
    }
    if( f < 1.0 )
    {
        return ( ( 12 - 9 * B - 6 * C ) * ( f * f * f ) +
               ( -18 + 12 * B + 6 *C ) * ( f * f ) +
               ( 6 - 2 * B ) ) / 6.0;
    }
    else if( f >= 1.0 && f < 2.0 )
    {
        return ( ( -B - 6 * C ) * ( f * f * f )
               + ( 6 * B + 30 * C ) * ( f *f ) +
               ( - ( 12 * B ) - 48 * C  ) * f +
               8 * B + 24 * C)/ 6.0;
    }
    else
    {
        return 0.0;
    }
}

float PlotInterpCurve::CatMullRom( float x )
{
    const float B = 0.0;
    const float C = 0.5;
    float f = x;
    if( f < 0.0 )
    {
        f = -f;
    }
    if( f < 1.0 )
    {
        return ( ( 12 - 9 * B - 6 * C ) * ( f * f * f ) +
            ( -18 + 12 * B + 6 *C ) * ( f * f ) +
            ( 6 - 2 * B ) ) / 6.0;
    }
    else if( f >= 1.0 && f < 2.0 )
    {
        return ( ( -B - 6 * C ) * ( f * f * f )
            + ( 6 * B + 30 * C ) * ( f *f ) +
            ( - ( 12 * B ) - 48 * C  ) * f +
            8 * B + 24 * C)/ 6.0;
    }
    else
    {
        return 0.0;
    }
}

float PlotInterpCurve::GetValue( float fIn )
{
    switch( m_eInterpolationType )
    {
        case INTERP_GLSL_BI_CUBIC_CATMULL:
        {
            return CatMullRom( fIn );
            break;
        }
        case INTERP_GLSL_BI_CUBIC_BELL:
        {
            return BellFunc( fIn );
            break;
        }
        case INTERP_GLSL_BI_CUBIC_B_SPLINE:
        {
            return BSpline( fIn );
            break;
        }
        case INTERP_GLSL_BI_CUBIC_TRIANGULAR:
        {
            return Triangular( fIn );
            break;
        }
    }
    return 0;
}

void PlotInterpCurve::Draw()
{
    RECT Rect;
    CWnd::FromHandle( m_hDrawWindow )->GetClientRect( &Rect );
    const int MAX = Rect.right;
    CDC* pDC = CWnd::FromHandle( m_hDrawWindow )->GetDC();
    pDC->FillSolidRect( 0,0, MAX,130, RGB( 255, 255,255 ) );

    if( m_eInterpolationType < INTERP_GLSL_BI_CUBIC_TRIANGULAR )
    {
        ReleaseDC( m_hDrawWindow, pDC->m_hDC );
        ::ValidateRect(m_hDrawWindow, 0 );
        return;
    }

    // Plot graph divisioans.
    CPen DashedPen;
    DashedPen.CreatePen( PS_DOT, 1, RGB( 0, 0 , 255 ) );
    HPEN hOldPen = (HPEN)pDC->SelectObject( DashedPen );

    // Horizontal line
    pDC->MoveTo( 0, 100 );
    pDC->LineTo( MAX, 100 );

    // Verical Line
    pDC->MoveTo( MAX/2, 0 );
    pDC->LineTo( MAX/2, 130 );

    // Select old pen
    pDC->SelectObject( hOldPen );

    float fMAX = 4.0;
    float fIncrFactor = fMAX / MAX;
    int nX = 0;
    pDC->MoveTo( 0, 100 );
    for (float fStart = -2; fStart < 2; fStart+= fIncrFactor )
    {
        float fFactor = GetValue( fStart );
        int nY = 100 - fFactor * 100;
        pDC->LineTo( nX++, nY );
    }

    // Preparing Co-ordinate values.
    RECT Rect_Left = { 0,100, 30, 130 };
    RECT Rect_Right = { MAX - 30,100, MAX, 130 };
    RECT Rect_Top = { MAX/2+10,0, MAX/2 + 40, 20 };
    RECT Rect_Center = { MAX/2+10,100, MAX/2 + 40, 130 };

    pDC->SetBkMode( TRANSPARENT );

    HFONT FontObj = CreateFontA( 12, 6, 0, 0, FW_DONTCARE, 0, 0, 0, ANSI_CHARSET, 
        OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        DEFAULT_PITCH  , "Arial" );

    HFONT hOldFont = ( HFONT )pDC->SelectObject( FontObj );
    pDC->SetTextColor( RGB( 0, 0, 255 ));
    pDC->DrawText( "-2.0", &Rect_Left, 0 );
    pDC->DrawText( "+2.0", &Rect_Right, 0 );
    pDC->DrawText( "0.0", &Rect_Center, 0 );
    pDC->DrawText( "1.0", &Rect_Top, 0 );

    pDC->SelectObject( hOldFont );

    ReleaseDC( m_hDrawWindow, pDC->m_hDC );

    ::ValidateRect(m_hDrawWindow, 0 );
}