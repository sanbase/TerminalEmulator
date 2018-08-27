#if !defined(AFX_TELNETSOCKET_H__B6441C0A_E0FB_47AF_9C3B_B6EDE5A69988__INCLUDED_)
#define AFX_TELNETSOCKET_H__B6441C0A_E0FB_47AF_9C3B_B6EDE5A69988__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TelnetSocket.h : header file
//
#include <Afxsock.h>
class telnetIO;

/////////////////////////////////////////////////////////////////////////////
// TelnetSocket command target

class TelnetSocket : public CAsyncSocket
{
// Attributes
public:

// Operations
public:
	TelnetSocket(telnetIO *);
	virtual ~TelnetSocket();

// Overrides
public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(TelnetSocket)
	public:
	virtual void OnReceive(int nErrorCode);
	virtual void OnClose(int nErrorCode);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(TelnetSocket)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

// Implementation
protected:
	telnetIO *io;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TELNETSOCKET_H__B6441C0A_E0FB_47AF_9C3B_B6EDE5A69988__INCLUDED_)
