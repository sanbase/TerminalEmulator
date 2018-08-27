// TelnetSocket.cpp : implementation file
//

#include "stdafx.h"
#include "terminal.h"
#include "TelnetSocket.h"
#include "telnetIO.h"
#include "terminalView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// TelnetSocket

TelnetSocket::TelnetSocket(telnetIO *io)
{
	this->io=io;
	BOOL opt=true;
	this->SetSockOpt(TCP_NODELAY,&opt,sizeof opt);	
}

TelnetSocket::~TelnetSocket()
{
}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(TelnetSocket, CAsyncSocket)
	//{{AFX_MSG_MAP(TelnetSocket)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// TelnetSocket member functions

void TelnetSocket::OnReceive(int nErrorCode) 
{
	// TODO: Add your specialized code here and/or call the base class
	io->ProcessMessage(this);	
//	CAsyncSocket::OnReceive(nErrorCode);
}

void TelnetSocket::OnClose(int nErrorCode) 
{
	// TODO: Add your specialized code here and/or call the base class
	io->connected=false;
	CAsyncSocket::OnClose(nErrorCode);
#ifdef CX5
	AfxMessageBox("ConteXt close session");
	CWnd *win=io->term->main_form->GetActiveWindow();
	win->DestroyWindow();
#else
	AfxMessageBox(io->m_host+":Connection close");
#endif
}
