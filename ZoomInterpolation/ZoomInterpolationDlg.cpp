// ZoomInterpolationDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ZoomInterpolation.h"
#include "ZoomInterpolationDlg.h"
#include "BMPLoader.h"
#include "GLSLShader.h"
#include "GLText.h"
#include "gdiplus.h"
#include "GdiplusImaging.h"
#include "Gdipluspixelformats.h"
#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CZoomInterpolationDlg dialog

CZoomInterpolationDlg::CZoomInterpolationDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CZoomInterpolationDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CZoomInterpolationDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

    m_gdiplusToken = 0;
    m_pShader = 0;
    m_eInterpolationType = INTERP_LINEAR;
}

CZoomInterpolationDlg::~CZoomInterpolationDlg()
{
    for( int nViewIndex = 0; nViewIndex < m_ImageView.size(); nViewIndex++ )
    {
        delete m_ImageView[nViewIndex];
    }
    if( m_pShader )
    {
        delete m_pShader;
        m_pShader = 0;
    }
    if( m_gdiplusToken )
    {
        Gdiplus::GdiplusShutdown( m_gdiplusToken );
    }
    CoUninitialize();
}

void CZoomInterpolationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CZoomInterpolationDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CZoomInterpolationDlg, CDialog)
	//{{AFX_MSG_MAP(CZoomInterpolationDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
    ON_WM_MOUSEMOVE()
    ON_WM_MOUSEWHEEL()
    ON_CONTROL_RANGE( BN_CLICKED, IDC_RADIO_GL_NEAREST, IDC_RADIO_ALL_IN_ONE, &CZoomInterpolationDlg::OnInterpolationChange )
    ON_BN_CLICKED(IDC_BUTTON1, &CZoomInterpolationDlg::OnLoadImage)
    ON_BN_CLICKED(IDC_BUTTON2, &CZoomInterpolationDlg::OnBnClickedButton2)
    ON_WM_TIMER()
    ON_WM_SETFOCUS()
    ON_BN_CLICKED(IDC_BUTTON3, &CZoomInterpolationDlg::OnZoomMinus)
    ON_BN_CLICKED(IDC_BUTTON4, &CZoomInterpolationDlg::OnZoomPlus)
    ON_BN_CLICKED(IDC_BUTTON_SAVE, &CZoomInterpolationDlg::OnBnClickedButtonSave)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CZoomInterpolationDlg message handlers

BOOL CZoomInterpolationDlg::OnInitDialog()
{
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	

    GetDlgItem( IDC_STATIC_RENDER )->SetWindowPos( 0, 0, 0, 800 + 208, 600, SWP_NOMOVE | SWP_NOZORDER | SWP_SHOWWINDOW );

	// OpenGL initialization.
    m_glSetup.InitGL( GetDlgItem( IDC_STATIC_RENDER )->m_hWnd );

    bool bExtensionFailed = false;
    CString csError = L"";

    if( !m_glSetup.CheckExtension( "GL_ARB_fragment_shader" ))
    {
        csError = L"\n GL_ARB_fragment_shader";
        bExtensionFailed = true;
    }
    if( !m_glSetup.CheckExtension( "GL_ARB_vertex_shader" ))
    {
        csError += L"\n GL_ARB_vertex_shader";
        bExtensionFailed = true;
    }



    BMPLoader BMPLoadObj;
    BYTE* pbyData = 0;
    BMPLoadObj.LoadBMP( IDB_BITMAP_FLOWER, m_nImageWidth, m_nImageHeight, pbyData );
    m_glTexture.Create( m_nImageWidth, m_nImageHeight, pbyData );
    delete[] pbyData;

    m_glVertexBuffer.CreateQuadVertexBuffer();

    // Set offset for displaying two flowers.
    m_fXOffset = 0.46193072;
    m_fYOffset = 0.24252957;
    m_fZoomWidth = 0.14254469;
    m_fZoomHeight = 0.14254469;

    // Prepare vertex buffer for zoomed texture mapping.
    PrepareVertexBuffer();

    ((CButton*)GetDlgItem( IDC_RADIO_GL_LINEAR ))->SetCheck( true );

    m_pShader = 0;

    // After Initialization of OpenGL, Extensions are loaded.
    TCHAR* pFailedFunction = 0;
    if( !GLExtension::GetInstance().GetWglProcAddress( pFailedFunction ))
    {
        // Any function pointer is not available.
        if( 0 != pFailedFunction )
        {
            bExtensionFailed = true;
        }
    }

    if( bExtensionFailed )
    {
        AfxMessageBox( CString( L"Following Extensions are not supported." ) + csError );

        // Here disable shader options in Interpolation Type menu.
        ((CButton*)GetDlgItem( IDC_RADIO_GLSL_LINEAR ))->EnableWindow( false );
        ((CButton*)GetDlgItem( IDC_RADIO_GLSL_BICUBIC_TRIANGULAR ))->EnableWindow( false );
        ((CButton*)GetDlgItem( IDC_RADIO_GLSL_BICUBIC_BELL ))->EnableWindow( false );
        ((CButton*)GetDlgItem( IDC_RADIO_GLSL_BICUBIC_BSPLINE ))->EnableWindow( false );
    }

    m_PlotCurve.SetDrawWindow( GetDlgItem( IDC_STATIC_INTERP_CURVE )->m_hWnd );
    m_PlotCurve.SetInterpolationType(  m_eInterpolationType );

    GetDlgItem( IDC_STATIC_RENDER )->GetClientRect( &m_ImageArea );

    ClientToScreen( &m_ImageArea );

    // This timer is added only to avoid a painting issue in some machines :)
    // Not interested to debug more and fix this issue.
    SetTimer( 1, 200, 0 ); // Only render in 5 fps.

    m_FontEngine.Create( GetDlgItem( IDC_STATIC_RENDER )->GetDC()->m_hDC );


    m_eTextureFilter = GL_LINEAR;
    PrepareSingleView();
    CoInitializeEx(NULL, COINIT_MULTITHREADED);

    return TRUE;  // return TRUE  unless you set the focus to a control
}

void CZoomInterpolationDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CZoomInterpolationDlg::OnPaint() 
{
    if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
        CDialog::OnPaint();
        RenderScene();

        // To plot the interpolation curve.
        m_PlotCurve.Draw();

        // Painting of Render Window is not required.
        GetDlgItem( IDC_STATIC_RENDER )->ValidateRect( 0 );
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CZoomInterpolationDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

// This function prepares entire image required for displaying a frame.
// Here Zoomed Image and Actual image are drawn.
// Finally image is blit in to screen.
void CZoomInterpolationDlg::RenderScene()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    // Draw zoomed image area with required shader.
    DrawZoomedImage();

    // Draw Actual image with red rectangle indicate Zoom area.
    DrawActualImage();

    // Completed draw operation, Now put to screen with SwapBuffer.
    m_glSetup.Draw();
}

/*
 Mouse movement changes zoomed image area, and creates a Pan effect.
 Here new zoomed X offset and Y offset are calculated.
 If new offset cause displaying outside data, this function revert the changes.
 */
void CZoomInterpolationDlg::OnMouseMove(UINT nFlags, CPoint point)
{
    point -= CPoint( m_ImageArea.left, m_ImageArea.top );
    static CPoint PointStart;
    static bool bMoveStart = false;
    if( nFlags == MK_LBUTTON && !bMoveStart && WithinImageArea( point ))
    {
        bMoveStart = true;
        PointStart = point;
    }
    else if( bMoveStart && nFlags == MK_LBUTTON )
    {
        // Move 
        int nXDiff = - ( point.x - PointStart.x );
        int nYDiff = point.y - PointStart.y;
        PointStart = point;

        // If mouse move in X direction
        if( nXDiff )
        {
            float fOldX = m_fXOffset;
            m_fXOffset += float( nXDiff )/ float( m_nImageWidth );

            // If new change display data outside of texture, then reset to old value.
            if( m_fXOffset < 0.0 )
            {
                m_fXOffset = 0.0;
            }
            if( m_fXOffset + m_fZoomWidth > 1.0 )
            {
                m_fXOffset = fOldX;
            }
        }

        // If mouse move in Y direction
        if( nYDiff )
        {
            float fOldY = m_fYOffset;
            m_fYOffset += float( nYDiff ) / float( m_nImageHeight );

            // If new change display data outside the texture, reset to old value.
            if( m_fYOffset < 0.0 )
            {
                m_fYOffset = 0.0;
            }
            if( m_fYOffset + m_fZoomHeight > 1.0 )
            {
                m_fYOffset = fOldY;
            }
        }

        // Cretae new vertex buffer for zoomed texture mapping.
        PrepareVertexBuffer();

        // Requesting a repaint.
        Invalidate( false );

    }
    else
    {
        bMoveStart = false;

        // Check mouse area is within image area then, set Arrow cursor.
        if( WithinImageArea( point ))
        {
            ::SetCursor( LoadCursor( 0, IDC_SIZEALL ));
        }
        else
        {
            ::SetCursor( LoadCursor( 0, IDC_ARROW ));
        }
    }
    CDialog::OnMouseMove(nFlags, point);
}

// USed to identify the pan is applied within image area.
bool CZoomInterpolationDlg::WithinImageArea( CPoint point )
{
    return( point.x > 0 && point.x < 800 &&
            point.y > 0 && point.y < 600 );
}

/*
 This function increase or decrease the zoomed area. 
 */
BOOL CZoomInterpolationDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    float fZoomValue = float( zDelta ) / WHEEL_DELTA;
    HandleZoom( fZoomValue );
    return CDialog::OnMouseWheel(nFlags, zDelta, pt);
}

/*
 This function handles change in interpolation type.
 When user changes interpolation type from a shader mode shader off,
 GLSLShader instance will be deleted.

 And when switching from shader off( IDC_RADIO_GL_NEAREST) to 
 shader on(IDC_RADIO_GLSL_LINEAR) case, new shader will be created.
 */

void CZoomInterpolationDlg::OnInterpolationChange(UINT nID)
{
    if( 0 != m_pShader )
    {
        delete m_pShader;
        m_pShader = 0;
    }
    // Consider Special case. All in one display.
    if( nID == IDC_RADIO_ALL_IN_ONE )
    {
        m_eInterpolationType = INTERP_NEAREST;
        m_PlotCurve.SetInterpolationType( INTERP_NEAREST );
        PrepareAllInOneView();
        Invalidate( false );
        return;
    }

    // If switching from All in one view to Normal View,
    // Delete all old Views.
    if( 1 < m_ImageView.size())
    {
        // Delete unwanted views.
        PrepareSingleView();
    }

    if( nID == IDC_RADIO_GL_NEAREST )
    {
        m_eInterpolationType = INTERP_NEAREST;
        m_glTexture.SetFilterType( GL_TEXTURE_MAG_FILTER, GL_NEAREST );
        m_eTextureFilter = GL_NEAREST;
    }
    else if( nID == IDC_RADIO_GL_LINEAR )
    {
        m_eInterpolationType = INTERP_LINEAR;
        m_glTexture.SetFilterType( GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        m_eTextureFilter = GL_LINEAR;
    }
    else
    {
        // Shader create is required.
        int nShaderProgramID = 0;
        if( IDC_RADIO_GLSL_LINEAR == nID )
        {
            m_eInterpolationType = INTERP_GLSL_LINEAR;
            nShaderProgramID = IDR_GLSL_SHADER_BILINEAR;
        }
        else if( IDC_RADIO_GLSL_BICUBIC_TRIANGULAR == nID )
        {
            m_eInterpolationType = INTERP_GLSL_BI_CUBIC_TRIANGULAR;
            nShaderProgramID = IDR_GLSL_SHADER_BICUBIC;
        }
        else if( IDC_RADIO_GLSL_BICUBIC_BELL == nID )
        {
            m_eInterpolationType = INTERP_GLSL_BI_CUBIC_BELL;
            nShaderProgramID = IDR_GLSL_SHADER_BELL;
        }
        else if( IDC_RADIO_GLSL_BICUBIC_BSPLINE == nID )
        {
            m_eInterpolationType = INTERP_GLSL_BI_CUBIC_B_SPLINE;
            nShaderProgramID = IDR_GLSL_SHADER_BSPLINE;
        }
        else if( IDC_RADIO_GLSL_BICUBIC_CATMULL == nID )
        {
            m_eInterpolationType = INTERP_GLSL_BI_CUBIC_CATMULL;
            nShaderProgramID = IDR_GLSL_SHADER_CATMULL;
        }
        m_glTexture.SetFilterType( GL_TEXTURE_MAG_FILTER, GL_NEAREST );
        m_pShader = new GLSLShader();
        m_pShader->CreateProgram( nShaderProgramID, GL_FRAGMENT_PROGRAM_ARB );
    }
    m_PlotCurve.SetInterpolationType( m_eInterpolationType );

    for( int nViewIndex = 0; nViewIndex < m_ImageView.size(); nViewIndex++ )
    {
        (m_ImageView[nViewIndex])->SetShader( m_pShader );
        (m_ImageView[nViewIndex])->SetTextureParam( &m_glTexture, m_nImageWidth,
                                                     m_nImageHeight, m_eTextureFilter );
    }

    // To plot the interpolation curve.
    m_PlotCurve.Draw();

    Invalidate( false );
}
void CZoomInterpolationDlg::OnLoadImage()
{
    // Create a file open Dialog for opening .bmp file.
    CFileDialog* pFileOpenDlg = new CFileDialog( TRUE,L"sdat", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
    L"Input Image (*.bmp;*.jpg;*.png;*.tga)|*.bmp;*.jpg;*.png;*.tga||");
    if( IDOK == pFileOpenDlg->DoModal())
    {
        CString csFileName = pFileOpenDlg->GetPathName();
        int nWidth = 0;
        int nHeight = 0;
        BYTE* pbyData = 0;
        BMPLoader BMPLoaderObj;
        if( !BMPLoaderObj.LoadBMP( csFileName.GetBuffer( 0 ), nWidth, nHeight, pbyData ))
        {
            AfxMessageBox( L"BMP Loading failed" );
            return;
        }
        if( 0 == pbyData )
        {
            AfxMessageBox( L"Memory Allocation failed." );
            return;
        }
        m_nImageWidth = nWidth;
        m_nImageHeight = nHeight;
        m_glTexture.Delete();
        if( !m_glTexture.Create( nWidth, nHeight, pbyData ))
        {
            AfxMessageBox( L"Texture loading failed" );
            return;
        }
        delete[] pbyData;
        csFileName.ReleaseBuffer();

        // Change Interpolation type of texture based on current interpolation type selected.
        if( INTERP_LINEAR != m_eInterpolationType )
        {
            m_glTexture.SetFilterType( GL_TEXTURE_MAG_FILTER, GL_NEAREST );
        }

        Invalidate( false );
    }

    if( m_ImageView.size() > 1 )
    {
        // Create all shaders again.
        PrepareAllInOneView();
    }
    delete pFileOpenDlg;
}
/*
 Creating zoomed image.
 */
void CZoomInterpolationDlg::DrawZoomedImage()
{
    // Displays all views. Draw()_ method of GLImageView prepares the 
    // Zoomed image of a view. m_ImageView list holds single object in normal case.
    // When All In View is selected in Interpolation type, m_ImageView will hold 7 View
    // objects.
    for( int nViewIndex = 0; nViewIndex < m_ImageView.size(); nViewIndex++ )
    {
        (m_ImageView[nViewIndex])->Draw();
    }
}
/*
 This function draws miniature of actual image with a Red region
 indicating the zoomed area.
 */
void CZoomInterpolationDlg::DrawActualImage()
{
    // Set Rendering area of Actual image.
    glViewport( 805, 10, 200, 150 );

    // Image is attached.
    m_glTexture.Enable();

    // Entire image is mapped to screen.
    m_glVertexBuffer.DrawVertexBuffer( GL_QUADS );
    m_glTexture.Disable();

    // Set Red color for Zoom area indication.
    glColor3f( 1.0, 0.0, 0.0 );

    float fXStart = m_fXOffset * 2;
    float fYStart = m_fYOffset * 2;
    float fWidth = m_fZoomWidth * 2;
    float fHeight = m_fZoomHeight * 2;

    // Draw a rectangle indicate zoom area.
    glBegin( GL_LINE_LOOP );
    glVertex2d( -1.0 + fXStart         ,  -1.0 + fYStart );
    glVertex2d( -1.0 + fXStart + fWidth,  -1.0 + fYStart );
    glVertex2d( -1.0 + fXStart + fWidth,  -1.0 + fYStart + fHeight );
    glVertex2d( -1.0 + fXStart         ,  -1.0 + fYStart + fHeight );
    glVertex2d( -1.0 + fXStart         ,  -1.0 + fYStart );
    glEnd();

    glColor3f( 0.0, 0.0, 1.0 );

    glBegin( GL_LINE_LOOP );

    static float fPixel = 1.0 / 600.0;
    glVertex2d( -1+ fPixel, -1 + fPixel);
    glVertex3f( 1, -1 + fPixel, 0.0 );

    glVertex3f( 1, 1 , 0.0);
    glVertex3f( -1, 1 , 0.0);
    glVertex3f( -1 + fPixel, -1, 0.0 );
    glEnd();
    glEnable( GL_TEXTURE_2D );
    glColor3f( 1.0, 1.0, 1.0 );
}

// About message
void CZoomInterpolationDlg::OnBnClickedButton2()
{
    CAboutDlg dlg;
    dlg.DoModal();
}

/*
 Function to create Vertex Buffer with current zoom parameters.
 */
void CZoomInterpolationDlg::PrepareVertexBuffer()
{
    const float fXOffset = m_fXOffset;
    const float fYOffset = m_fYOffset;
    const float fWidth = m_fZoomWidth;
    const float fHeight = m_fZoomHeight;
    m_glVertexBufferZoom.SetAt( 0, -1.0f,1.0f, 0.0f,    fXOffset, fHeight + fYOffset ); // Left Top  corner
    m_glVertexBufferZoom.SetAt( 1, -1.0f,-1.0f, 0.0f,   fXOffset, fYOffset );// Left Bottom
    m_glVertexBufferZoom.SetAt( 2, 1.0f , -1.0f, 0.0f,  fXOffset + fWidth, fYOffset); // Right bottom
    m_glVertexBufferZoom.SetAt( 3, 1.0f, 1.0f,  0.0f,   fXOffset + fWidth, fHeight + fYOffset ); // Right top
}
void CZoomInterpolationDlg::OnTimer(UINT_PTR nIDEvent)
{
    // Rendering a frame.
    RenderScene();
    CDialog::OnTimer(nIDEvent);
}

void CZoomInterpolationDlg::OnSetFocus(CWnd* pOldWnd)
{
    CDialog::OnSetFocus(pOldWnd);

    RenderScene();
    m_PlotCurve.Draw();
}


// Handles Zoom
void CZoomInterpolationDlg::HandleZoom( const float fZoomDelta_i )
{
    float fZoomXIncr = m_fZoomHeight * 0.025;
    float fZoomYIncr = m_fZoomWidth * 0.025;

    // Zoom
    if( fZoomDelta_i < 0.0 )
    {

        /*
         Increase of Zoomed area in Left and Right will be same.
         That is implemented through decrementing offset and increasing the width.
         In effect equal increase in X and Y directional will be achieved.
         */
        m_fXOffset -= fZoomXIncr / 2;
        m_fYOffset -= fZoomYIncr / 2;

        m_fZoomHeight += fZoomYIncr;
        m_fZoomWidth += fZoomXIncr;

        // Validating new zoom region.
        if( m_fYOffset + m_fZoomHeight > 1.0 || m_fXOffset + m_fZoomWidth > 1.0 ||
            m_fYOffset < 0.0 || m_fXOffset < 0.0 )
        {
            // Revert the change.
            m_fXOffset += fZoomXIncr / 2;
            m_fYOffset += fZoomYIncr / 2;

            m_fZoomHeight -= fZoomYIncr;
            m_fZoomWidth -= fZoomXIncr;
        }
    }
    else
    {
        // Zoom
        m_fXOffset += fZoomXIncr / 2;
        m_fYOffset += fZoomYIncr / 2;

        m_fZoomHeight -= fZoomYIncr;
        m_fZoomWidth -= fZoomXIncr;

        // Validating new zoom region.
        // Unzooom is possible from a correct zoom region therefore 
        // validating the minimum size is OK.
        if( m_fZoomWidth < 0.005 )
        {
            m_fZoomWidth = 0.005;
        }
        if( m_fZoomHeight < 0.005 )
        {
            m_fZoomHeight = 0.005;
        }
    }

    // Prepare vertex buffer for zoomed texture mapping.
    PrepareVertexBuffer();
    Invalidate( false );
}

void CZoomInterpolationDlg::OnZoomMinus()
{
    HandleZoom( -1 );
}

void CZoomInterpolationDlg::OnZoomPlus()
{
    HandleZoom( 1 );
}


void CZoomInterpolationDlg::PrepareSingleView()
{
    for( int nViewIndex = 0; nViewIndex < m_ImageView.size(); nViewIndex++ )
    {
        delete m_ImageView[nViewIndex];
    }
    m_ImageView.clear();

    GLImageView* pImage = new GLImageView();
    pImage->SetViewport( 0, 0, 800, 600 );
    pImage->SetShader( m_pShader );
    pImage->SetTextureParam( &m_glTexture, m_nImageWidth, m_nImageHeight, m_eTextureFilter );
    pImage->SetVertexBuffer( &m_glVertexBufferZoom );
    m_ImageView.push_back( pImage );
}

void CZoomInterpolationDlg::PrepareAllInOneView()
{
    for( int nViewIndex = 0; nViewIndex < m_ImageView.size(); nViewIndex++ )
    {
        delete m_ImageView[nViewIndex];
    }
    m_ImageView.clear();

    if( m_pShader )
    {
        delete m_pShader;
        m_pShader = 0;
    }

    // Hold Name of different Interpolation.
    TCHAR* INTERPOLATION_NAMES[] =
    {
        L"GL NEAREST",
        L"GL BILINEAR",
        L"GLSL BILINEAR",
        L"GLSL BI-CUBIC TRIANGULAR",
        L"GLSL BI-CUBIC BELL",
        L"GLSL BI-CUBIC BSPLINE",
        L"GLSL BI-CUBIC CATMULL"
    };

    int nViewPortXDiv = 800 / 3;
    int nViewPortYDiv = 600 / 3;
    int nResourceId = IDR_GLSL_SHADER_BILINEAR;

    for( int nI = 0; nI < 7; nI++ )
    {
        int nX = nI % 3;
        int nY = nI / 3;

        GLText* pText = new GLText( &m_FontEngine );
        pText->SetText( INTERPOLATION_NAMES[nI] );

        GLImageView* pImage = new GLImageView();
        pImage->SetViewport( nX * nViewPortXDiv, nY * nViewPortYDiv, nViewPortXDiv, nViewPortYDiv );
        pImage->SetText( pText );
        GLenum eTextureFilter;
        if( nI >= 2 )
        {
            GLSLShader* pTempShader = new GLSLShader();

            pTempShader->CreateProgram( nResourceId++, GL_FRAGMENT_PROGRAM_ARB );
            pImage->SetShader( pTempShader, true );
            eTextureFilter = GL_NEAREST;
        }
        else
        {
            pImage->SetShader( 0 );
            if( 0 == nI )
            {
                eTextureFilter = GL_NEAREST;
            }
            else
            {
                eTextureFilter = GL_LINEAR;
            }
        }
        pImage->SetTextureParam( &m_glTexture, m_nImageWidth, m_nImageHeight, eTextureFilter );
        pImage->SetVertexBuffer( &m_glVertexBufferZoom );
        m_ImageView.push_back( pImage );
    }
}
void CZoomInterpolationDlg::OnBnClickedButtonSave()
{
    CString csFileName;
    csFileName.Format( L"ZoomInterpolation.bmp" );
    CFileDialog SaveDlg( false, L"*.bmp", csFileName );
    if( IDOK == SaveDlg.DoModal())
    {
        RECT stImageArea;
        stImageArea.left =0;
        stImageArea.top = 0;
        stImageArea.right = 800;
        stImageArea.bottom = 600;
        CString csFileName = SaveDlg.GetPathName();
        BYTE* pbyData = new BYTE[stImageArea.bottom * stImageArea.right  * 3];
        if( 0 == pbyData )
        {
            AfxMessageBox( L"Memory Allocation failed" );
            return;
        }
        glReadPixels( 0, 0, stImageArea.right, stImageArea.bottom, GL_BGR_EXT, GL_UNSIGNED_BYTE, pbyData );
        BMPLoader SaveBmp;
        SaveBmp.SaveBMP( csFileName, stImageArea.right, stImageArea.bottom, pbyData );
        delete[] pbyData;
    }
}
