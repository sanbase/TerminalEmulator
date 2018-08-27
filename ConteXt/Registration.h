#if !defined(AFX_REGISTRATION_H__1A6F9318_D0A2_49BE_9033_BDA5D1D17B4D__INCLUDED_)
#define AFX_REGISTRATION_H__1A6F9318_D0A2_49BE_9033_BDA5D1D17B4D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Registration.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// Registration dialog

class Registration : public CDialog
{
// Construction
public:
	Registration(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(Registration)
	enum { IDD = IDD_DIALOG2 };
	long	m_SN;
	long	m_Key;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Registration)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(Registration)
	afx_msg void OnDone();
	afx_msg void OnRegistration();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REGISTRATION_H__1A6F9318_D0A2_49BE_9033_BDA5D1D17B4D__INCLUDED_)
