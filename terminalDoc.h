// terminalDoc.h : interface of the CTerminalDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_TERMINALDOC_H__E2F6EBCD_C2F2_4F45_BDBE_FC3FF64037EC__INCLUDED_)
#define AFX_TERMINALDOC_H__E2F6EBCD_C2F2_4F45_BDBE_FC3FF64037EC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CTerminalDoc : public CDocument
{
protected: // create from serialization only
	CTerminalDoc();
	DECLARE_DYNCREATE(CTerminalDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTerminalDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTerminalDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CTerminalDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TERMINALDOC_H__E2F6EBCD_C2F2_4F45_BDBE_FC3FF64037EC__INCLUDED_)
