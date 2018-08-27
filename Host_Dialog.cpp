// Host_Dialog.cpp : implementation file
//

#include "stdafx.h"
#include "terminal.h"
#include "Host_Dialog.h"
#include <io.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
//#include <stdlib.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Host_Dialog dialog
extern int current_screen;
char *get_str(char *,int);

Host_Dialog::Host_Dialog(CWnd* pParent /*=NULL*/)
	: CDialog(Host_Dialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(Host_Dialog)
	//}}AFX_DATA_INIT

	m_host[0] = _T("unixspace.com");
	m_host[1] = _T("unixspace.com");
	m_host[2] = _T("unixspace.com");
	m_host[3] = _T("unixspace.com");

	m_term[0] = _T("Linux");
	m_term[1] = _T("FreeBSD");
	m_term[2] = _T("VT220");
	m_term[3] = _T("ANSI");

	m_prot[0] = _T("telnet");
	m_prot[1] = _T("SSH");
	m_prot[2] = _T("rlogin");
	m_prot[3] = _T("raw");

	m_lines[0] = 32;
	m_lines[1] = 32;
	m_lines[2] = 32;
	m_lines[3] = 32;

	Parent=pParent; 
	struct stat st;
	char *buf=NULL;
	int len;
	CString host;
	int fd=open(path+"etc/terminal.host",_O_RDONLY|_O_BINARY);
	if(fd>0)
	{
		fstat(fd,&st);
		len=st.st_size;
		buf=(char *)calloc(len+1,1);
		read(fd,buf,len);
		close(fd);
		for(int i=0;i<4;i++)
		{
			char *ch=get_str(buf,i);
			int j;
			if(ch!=NULL)
			{
				j=sscanf(ch,"Host=%s Prot=%s Term=%s Lines=%d",m_host[i],m_prot[i],m_term[i],&m_lines[i]);
			}
		}
	}	
	free(buf);
}


void Host_Dialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(Host_Dialog)
	DDX_Text(pDX, IDC_HOST1, m_host[0]);
	DDX_Text(pDX, IDC_HOST2, m_host[1]);
	DDX_Text(pDX, IDC_HOST3, m_host[2]);
	DDX_Text(pDX, IDC_HOST4, m_host[3]);

	DDX_CBString(pDX, IDC_PROTOCOL1, m_prot[0]);
	DDX_CBString(pDX, IDC_PROTOCOL2, m_prot[1]);
	DDX_CBString(pDX, IDC_PROTOCOL3, m_prot[2]);
	DDX_CBString(pDX, IDC_PROTOCOL4, m_prot[3]);

	DDX_CBString(pDX, IDC_TERMINALLIST1, m_term[0]);
	DDX_CBString(pDX, IDC_TERMINALLIST2, m_term[1]);
	DDX_CBString(pDX, IDC_TERMINALLIST3, m_term[2]);
	DDX_CBString(pDX, IDC_TERMINALLIST4, m_term[3]);

	DDX_Text(pDX, IDC_LINES1, m_lines[0]);
	DDX_Text(pDX, IDC_LINES2, m_lines[1]);
	DDX_Text(pDX, IDC_LINES3, m_lines[2]);
	DDX_Text(pDX, IDC_LINES4, m_lines[3]);

	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(Host_Dialog, CDialog)
	//{{AFX_MSG_MAP(Host_Dialog)
	ON_BN_CLICKED(IDOK, OnConnect)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Host_Dialog message handlers

void Host_Dialog::OnConnect() 
{
	if (!UpdateData(TRUE))
	{
		AfxMessageBox("Dialog Error!");
		return;
	}
	int fd=_creat(path+"etc/terminal.host",_S_IREAD | _S_IWRITE);
	for(int i=0;i<4;i++)
	{
		char str[256];
		sprintf(str,"Host=%s Prot=%s Term=%s Lines=%d\n",m_host[i],m_prot[i],m_term[i],m_lines[i]);
		write(fd,str,strlen(str));
	}
	close(fd);	
	EndDialog(true);
}

void Host_Dialog::OnCancel() 
{
	// TODO: Add extra cleanup here
	int fd=open(path+"etc/terminal.host",_O_RDONLY|_O_BINARY);
	if(fd<0)
	{
		AfxMessageBox("Sorry, but this programm can not work without host setting.");
		exit(0);
	}
	close(fd);
	CDialog::OnCancel();
}
