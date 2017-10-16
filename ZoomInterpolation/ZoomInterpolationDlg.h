// ZoomInterpolationDlg.h : header file
//

#if !defined(AFX_ZOOMINTERPOLATIONDLG_H__7AFD9082_C3A0_4C2C_8F0C_F4ABAC726949__INCLUDED_)
#define AFX_ZOOMINTERPOLATIONDLG_H__7AFD9082_C3A0_4C2C_8F0C_F4ABAC726949__INCLUDED_

#include "GLSetup.h"
#include "GLTexture.h"
#include "GLVertexBuffer.h"
#include "PlotInterpCurve.h"
#include "Common.h"
#include "GLImageView.h"
#include "FontEngine.h"
#include <vector>

class GLSLShader;

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CZoomInterpolationDlg dialog

class CZoomInterpolationDlg : public CDialog
{
// Construction
public:
	CZoomInterpolationDlg(CWnd* pParent = NULL);	// standard constructor

    ~CZoomInterpolationDlg();

// Dialog Data
	//{{AFX_DATA(CZoomInterpolationDlg)
	enum { IDD = IDD_ZOOMINTERPOLATION_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CZoomInterpolationDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CZoomInterpolationDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnInterpolationChange(UINT nID);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

    void RenderScene();
    bool WithinImageArea( CPoint point );
    void DrawZoomedImage();
    void DrawActualImage();
    void PrepareVertexBuffer();
    void HandleZoom( const float fZoomDelta_i );
    void PrepareSingleView();
    void PrepareAllInOneView();



private:

    GLSetup m_glSetup;
    GLTexture m_glTexture;
    GLVertexBuffer m_glVertexBuffer;

    GLVertexBuffer m_glVertexBufferZoom;

    int m_nImageWidth;
    int m_nImageHeight;
    float m_fXOffset;
    float m_fYOffset;

    float m_fZoomWidth;
    float m_fZoomHeight;

    INTERPOLATION_TYPE_e m_eInterpolationType;

    GLSLShader* m_pShader;
    ULONG_PTR m_gdiplusToken;

    PlotInterpCurve m_PlotCurve;
    RECT            m_ImageArea;
    GLenum          m_eTextureFilter;
    FontEngine      m_FontEngine;
    std::vector<GLImageView*> m_ImageView;

public:

    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    afx_msg void OnLoadImage();
    afx_msg void OnBnClickedButton2();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnSetFocus(CWnd* pOldWnd);
    afx_msg void OnBnClickedRadioGlslBicubicCatmull();
    afx_msg void OnZoomMinus();
    afx_msg void OnZoomPlus();
    afx_msg void OnBnClickedButtonSave();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ZOOMINTERPOLATIONDLG_H__7AFD9082_C3A0_4C2C_8F0C_F4ABAC726949__INCLUDED_)
