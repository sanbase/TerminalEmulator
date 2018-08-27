// terminalDoc.cpp : implementation of the CTerminalDoc class
//

#include "stdafx.h"
#include "terminal.h"

#include "terminalDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTerminalDoc

IMPLEMENT_DYNCREATE(CTerminalDoc, CDocument)

BEGIN_MESSAGE_MAP(CTerminalDoc, CDocument)
	//{{AFX_MSG_MAP(CTerminalDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_FILE_SEND_MAIL, OnFileSendMail)
	ON_UPDATE_COMMAND_UI(ID_FILE_SEND_MAIL, OnUpdateFileSendMail)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTerminalDoc construction/destruction

CTerminalDoc::CTerminalDoc()
{
	// TODO: add one-time construction code here

}

CTerminalDoc::~CTerminalDoc()
{
}

BOOL CTerminalDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CTerminalDoc serialization

void CTerminalDoc::Serialize(CArchive& ar)
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
// CTerminalDoc diagnostics

#ifdef _DEBUG
void CTerminalDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CTerminalDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

