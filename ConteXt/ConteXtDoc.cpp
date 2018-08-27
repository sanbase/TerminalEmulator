// ConteXtDoc.cpp : implementation of the CConteXtDoc class
//

#include "stdafx.h"
#include "ConteXt.h"

#include "ConteXtDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConteXtDoc

IMPLEMENT_DYNCREATE(CConteXtDoc, CDocument)

BEGIN_MESSAGE_MAP(CConteXtDoc, CDocument)
	//{{AFX_MSG_MAP(CConteXtDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConteXtDoc construction/destruction

CConteXtDoc::CConteXtDoc()
{
	// TODO: add one-time construction code here

}

CConteXtDoc::~CConteXtDoc()
{
}

BOOL CConteXtDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CConteXtDoc serialization

void CConteXtDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CConteXtDoc diagnostics

#ifdef _DEBUG
void CConteXtDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CConteXtDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CConteXtDoc commands
