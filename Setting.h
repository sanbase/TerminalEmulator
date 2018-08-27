// setting.h: interface for the screen class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SETTING_H__B1E877EF_5099_414B_B7D7_C0CC48CB99C1__INCLUDED_)
#define AFX_SETTING_H__B1E877EF_5099_414B_B7D7_C0CC48CB99C1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Setting.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// Setting dialog

class Setting : public CDialog
{
// Construction
public:
	Setting(int *act, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(Setting)
	enum { IDD = IDD_DIALOG3 };
	CString	m_host1;
	CString	m_host2;
	CString	m_host3;
	CString	m_host4;
	int		m_lines1;
	int		m_lines2;
	int		m_lines3;
	int		m_lines4;
	CString	m_prot1;
	CString	m_prot2;
	CString	m_prot3;
	CString	m_prot4;
	CString	m_term1;
	CString	m_term2;
	CString	m_term3;
	CString	m_term4;
	int		m_port1;
	int		m_port2;
	int		m_port3;
	int		m_port4;
	CString	m_proxy_type;
	CString	m_proxy_host;
	int		m_proxy_port;
	int		m_act;
	CString	m_charset1;
	CString	m_charset2;
	CString	m_charset3;
	CString	m_charset4;
	//}}AFX_DATA

	CString *m_host;
	int *m_lines;
	CString *m_prot;
	CString *m_term;
	int *act;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Setting)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(Setting)
	afx_msg void OnCancel2();
	afx_msg void OnConnect();
	afx_msg void OnSave();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETTING_H__B1E877EF_5099_414B_B7D7_C0CC48CB99C1__INCLUDED_)
