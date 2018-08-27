// terminal.h : main header file for the TERMINAL application
//

#if !defined(AFX_TERMINAL_H__9E4C8EA9_B510_4FCA_B2D8_76DC1B73A931__INCLUDED_)
#define AFX_TERMINAL_H__9E4C8EA9_B510_4FCA_B2D8_76DC1B73A931__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include <gl\gl.h>										// Header File For The OpenGL32 Library
#include <gl\glu.h>										// Header File For The GLu32 Library
//#include <gl\glut.h>										// Header File For The GLu32 Library
#include <gl\glaux.h>
/////////////////////////////////////////////////////////////////////////////
// CTerminalApp:
// See terminal.cpp for the implementation of this class
//

extern CString path;
class CTerminalApp : public CWinApp
{
public:
	CTerminalApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTerminalApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CTerminalApp)
	afx_msg void OnAppAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TERMINAL_H__9E4C8EA9_B510_4FCA_B2D8_76DC1B73A931__INCLUDED_)
