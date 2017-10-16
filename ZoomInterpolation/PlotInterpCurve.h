#pragma once
#include "Common.h"

// Class for drawing current interpolation curve.
// When interpolation is GLSL_TRIANGULAR, or GLSL_BSPLINE, or GLSL_BELL
// this class plots the interpolation curve.
class PlotInterpCurve
{

public:

    PlotInterpCurve(void);
    ~PlotInterpCurve(void);
    void SetInterpolationType( const INTERPOLATION_TYPE_e eInterpolationType_i );
    void SetDrawWindow( const HWND hWindow_i );
    void Draw();

private:

    float GetValue( float fIn  );
    float BSpline( float x );
    float CatMullRom( float x );
    float Triangular( float x );
    float BellFunc( float x );

private:

    HWND                 m_hDrawWindow;
    INTERPOLATION_TYPE_e m_eInterpolationType;

};
