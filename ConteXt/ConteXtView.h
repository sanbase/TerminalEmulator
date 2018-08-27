// ConteXtView.h : interface of the CConteXtView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONTEXTVIEW_H__C57E8ABE_311C_4E9B_B044_FE24A1E70C28__INCLUDED_)
#define AFX_CONTEXTVIEW_H__C57E8ABE_311C_4E9B_B044_FE24A1E70C28__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "../TerminalView.h"

class CConteXtView : public CTerminalView
{
protected: // create from serialization only
	CConteXtView();
	DECLARE_DYNCREATE(CConteXtView)

// Attributes
public:
	CConteXtDoc* GetDocument();
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConteXtView)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual void OnInitialUpdate();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CConteXtView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	void Connect();
	void OnConteXtBrowser();
	void init();
protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CConteXtView)
	afx_msg void OnConnect();
	afx_msg void OnSetup();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in ConteXtView.cpp
inline CConteXtDoc* CConteXtView::GetDocument()
   { return (CConteXtDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONTEXTVIEW_H__C57E8ABE_311C_4E9B_B044_FE24A1E70C28__INCLUDED_)
