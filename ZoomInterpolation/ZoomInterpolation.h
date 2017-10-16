// ZoomInterpolation.h : main header file for the ZOOMINTERPOLATION application
//

#if !defined(AFX_ZOOMINTERPOLATION_H__81315F50_0441_40CA_98BB_95FE8B74573C__INCLUDED_)
#define AFX_ZOOMINTERPOLATION_H__81315F50_0441_40CA_98BB_95FE8B74573C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CZoomInterpolationApp:
// See ZoomInterpolation.cpp for the implementation of this class
//

class CZoomInterpolationApp : public CWinApp
{
public:
	CZoomInterpolationApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CZoomInterpolationApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CZoomInterpolationApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ZOOMINTERPOLATION_H__81315F50_0441_40CA_98BB_95FE8B74573C__INCLUDED_)
