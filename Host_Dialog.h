#if !defined(AFX_HOST_DIALOG_H__860F221F_B213_4307_AA40_D2CEC611B5C8__INCLUDED_)
#define AFX_HOST_DIALOG_H__860F221F_B213_4307_AA40_D2CEC611B5C8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Host_Dialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// Host_Dialog dialog

class Host_Dialog : public CDialog
{
// Construction
public:
	Host_Dialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(Host_Dialog)
	enum { IDD = IDD_DIALOG1 };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Host_Dialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(Host_Dialog)
	afx_msg void OnConnect();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CWnd* Parent;
	CString	m_host[4];
	CString	m_prot[4];
	CString	m_term[4];
	int	m_lines[4];
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HOST_DIALOG_H__860F221F_B213_4307_AA40_D2CEC611B5C8__INCLUDED_)
