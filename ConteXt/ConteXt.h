// ConteXt.h : main header file for the CONTEXT application
//

#if !defined(AFX_CONTEXT_H__6B998A16_5643_4E6C_8D4D_EAF79BE077C4__INCLUDED_)
#define AFX_CONTEXT_H__6B998A16_5643_4E6C_8D4D_EAF79BE077C4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CConteXtApp:
// See ConteXt.cpp for the implementation of this class
//

class CConteXtApp : public CWinApp
{
public:
	CConteXtApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConteXtApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CConteXtApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONTEXT_H__6B998A16_5643_4E6C_8D4D_EAF79BE077C4__INCLUDED_)
