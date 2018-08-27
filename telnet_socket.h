#if !defined(AFX_TELNET_SOCKET_H__CAEB25CD_DDD6_4509_8F34_03269E5400EE__INCLUDED_)
#define AFX_TELNET_SOCKET_H__CAEB25CD_DDD6_4509_8F34_03269E5400EE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// telnet_socket.h : header file
//
#include <afxsock.h>
class telnetIO;

/////////////////////////////////////////////////////////////////////////////
// telnet_socket command target

class telnet_socket : public CAsyncSocket
{
// Attributes
public:

// Operations
public:
	telnet_socket(telnetIO *);
	virtual ~telnet_socket();

// Overrides
public:
	telnetIO *term;
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(telnet_socket)
	public:
	virtual void OnClose(int nErrorCode);
	virtual void OnReceive(int nErrorCode);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(telnet_socket)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

// Implementation
protected:
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TELNET_SOCKET_H__CAEB25CD_DDD6_4509_8F34_03269E5400EE__INCLUDED_)
