// ConteXtDoc.h : interface of the CConteXtDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONTEXTDOC_H__FAA21BB1_3F41_42D6_969A_93F5DA856F80__INCLUDED_)
#define AFX_CONTEXTDOC_H__FAA21BB1_3F41_42D6_969A_93F5DA856F80__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CConteXtDoc : public CDocument
{
protected: // create from serialization only
	CConteXtDoc();
	DECLARE_DYNCREATE(CConteXtDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConteXtDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CConteXtDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CConteXtDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONTEXTDOC_H__FAA21BB1_3F41_42D6_969A_93F5DA856F80__INCLUDED_)
