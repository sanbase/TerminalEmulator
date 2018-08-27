// telnet_socket.cpp : implementation file
//

#include "stdafx.h"
#include "telnet_socket.h"
#include "telnetIO.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
// telnet_socket

telnet_socket::telnet_socket(telnetIO *io)
{
	term=io;
}

telnet_socket::~telnet_socket()
{
}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(telnet_socket, CAsyncSocket)
	//{{AFX_MSG_MAP(telnet_socket)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// telnet_socket member functions
void telnet_socket::OnClose(int nErrorCode) 
{
//	AfxMessageBox("Connection Closed",MB_OK);
	
	CAsyncSocket::OnClose(nErrorCode);
}

void telnet_socket::OnReceive(int nErrorCode) 
{
	term->ProcessMessage(this);
}
