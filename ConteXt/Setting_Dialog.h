#if !defined(AFX_SETTING_DIALOG_H__F7836373_BD72_477B_AE5B_B638D14F5381__INCLUDED_)
#define AFX_SETTING_DIALOG_H__F7836373_BD72_477B_AE5B_B638D14F5381__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Setting_Dialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// Setting_Dialog dialog

class Setting_Dialog : public CDialog
{
// Construction
public:
	Setting_Dialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(Setting_Dialog)
	enum { IDD = IDD_DIALOG1 };
	CString	m_term;
	int		m_lines;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Setting_Dialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(Setting_Dialog)
	afx_msg void OnSave();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETTING_DIALOG_H__F7836373_BD72_477B_AE5B_B638D14F5381__INCLUDED_)
