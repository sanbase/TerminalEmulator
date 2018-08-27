// Registration.cpp : implementation file
//

#include "stdafx.h"
#include "terminal.h"
#include "Registration.h"
#include <io.h>
#include <sys\stat.h>
#include <fcntl.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;  
#endif

/////////////////////////////////////////////////////////////////////////////
// Registration dialog

extern long GetVSNumber();

Registration::Registration(CWnd* pParent /*=NULL*/)
	: CDialog(Registration::IDD, pParent)
{
	//{{AFX_DATA_INIT(Registration)
	m_SN = 0;
	m_Key = 0;
	//}}AFX_DATA_INIT

	struct stat st;
	DWORD k;
	if(stat(path+"bin/UnixSpace.exe",&st)<0)
	{
		AfxMessageBox("The file UnixSpace.exe should be located in "+path+"bin/UnixSpace.exe");
		exit(0);
	}
	k=st.st_ctime+GetVSNumber();
	m_SN=k%673452;
}


void Registration::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(Registration)
	DDX_Text(pDX, IDC_EDIT2, m_SN);
	DDX_Text(pDX, IDC_EDIT1, m_Key);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(Registration, CDialog)
	//{{AFX_MSG_MAP(Registration)
	ON_BN_CLICKED(IDC_BUTTON1, OnDone)
	ON_BN_CLICKED(IDOK, OnRegistration)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Registration message handlers

void Registration::OnDone() 
{
	// TODO: Add your control notification handler code here
	int fd;
	if (!UpdateData(TRUE))
	{
		AfxMessageBox("Dialog Error!");
		return;
	}
	if((fd=creat(path+"etc/Terminal.key",_S_IREAD|_S_IWRITE))>0)
	{
		char str[64];
		sprintf(str,"%d",m_Key);
		write(fd,str,strlen(str));
		close(fd);
	}
	else
	{
		AfxMessageBox("Can't create file "+path+"etc/Terminal.key");
	}
	EndDialog(true);
}

void Registration::OnRegistration() 
{
	// TODO: Add your control notification handler code here
	char str[256];;
	sprintf(str,"http://www.unixspace.com/registration.html?%d",m_SN);
	ShellExecute(0,"open",str,NULL,NULL,SW_SHOWDEFAULT);	
//	EndDialog(true);
}
