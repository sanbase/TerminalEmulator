// Setting.cpp : implementation file
//

#include "stdafx.h"
#include "terminal.h"
#include "Setting.h"

#include <io.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
/*
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
*/
/////////////////////////////////////////////////////////////////////////////
// Setting dialog
extern char *get_str(char *,int);
extern int get_arg_line(CString line,char *pattern,char *arg);

Setting::Setting(int *act_std, CWnd* pParent /*=NULL*/)
	: CDialog(Setting::IDD, pParent)
{
	//{{AFX_DATA_INIT(Setting)
	m_host1 = _T("host.com");
	m_host2 = _T("host.com");
	m_host3 = _T("host.com");
	m_host4 = _T("host.com");
	m_lines1 = 32;
	m_lines2 = 32;
	m_lines3 = 25;
	m_lines4 = 42;
	m_prot1 = _T("telnet");
	m_prot2 = _T("SSH");
	m_prot3 = _T("rlogin");
	m_prot4 = _T("http-telnet");
	m_term1 = _T("Linux");
	m_term2 = _T("FreeBSD");
	m_term3 = _T("VT220");
	m_term4 = _T("UnixSpace");
	m_port1 = 0;
	m_port2 = 0;
	m_port3 = 0;
	m_port4 = 0;
	m_proxy_type = _T("");
	m_proxy_host = _T("");
	m_proxy_port = 0;
	m_act = -1;
	m_charset1 = _T("");
	m_charset2 = _T("");
	m_charset3 = _T("");
	m_charset4 = _T("");
	//}}AFX_DATA_INIT

	struct stat st;
	char *buf=NULL;
	int len;
	CString host;
	int fd=open(path+"etc/terminal.host",_O_RDONLY|_O_BINARY);
	m_act=*act_std;
	act=act_std;
	*act=-1;
	if(fd>0)
	{
		fstat(fd,&st);
		len=st.st_size;
		buf=(char *)calloc(len+1,1);
		read(fd,buf,len);
		close(fd);
		char *ch,str[64];

		ch=get_str(buf,0);
		if(ch!=NULL)
		{
			get_arg_line(ch,"Port",str);
			m_port1=atoi(str);
			get_arg_line(ch,"Lines",str);
			m_lines1=atoi(str);
			get_arg_line(ch,"Host",str);
			m_host1=str;
			get_arg_line(ch,"Term",str);
			m_term1=str;
			get_arg_line(ch,"Prot",str);
			m_prot1=str;
			get_arg_line(ch,"Charset",str);
			m_charset1=str;

		}
		ch=get_str(buf,1);
		if(ch!=NULL)
		{
			get_arg_line(ch,"Port",str);
			m_port2=atoi(str);
			get_arg_line(ch,"Lines",str);
			m_lines2=atoi(str);
			get_arg_line(ch,"Host",str);
			m_host2=str;
			get_arg_line(ch,"Term",str);
			m_term2=str;
			get_arg_line(ch,"Prot",str);
			m_prot2=str;
			get_arg_line(ch,"Charset",str);
			m_charset2=str;
		}
		ch=get_str(buf,2);
		if(ch!=NULL)
		{
			get_arg_line(ch,"Port",str);
			m_port3=atoi(str);
			get_arg_line(ch,"Lines",str);
			m_lines3=atoi(str);
			get_arg_line(ch,"Host",str);
			m_host3=str;
			get_arg_line(ch,"Term",str);
			m_term3=str;
			get_arg_line(ch,"Prot",str);
			m_prot3=str;
			get_arg_line(ch,"Charset",str);
			m_charset3=str;
		}
		ch=get_str(buf,3);
		if(ch!=NULL)
		{
			get_arg_line(ch,"Port",str);
			m_port4=atoi(str);
			get_arg_line(ch,"Lines",str);
			m_lines4=atoi(str);
			get_arg_line(ch,"Host",str);
			m_host4=str;
			get_arg_line(ch,"Term",str);
			m_term4=str;
			get_arg_line(ch,"Prot",str);
			m_prot4=str;
			get_arg_line(ch,"Charset",str);
			m_charset4=str;
		}
		ch=get_str(buf,4);
		if(ch!=NULL)
		{
			char str[64];
			int type=0;
			get_arg_line(ch,"ProxyType",str);
			if(*str)
			{
				switch((type=atoi(str)))
				{
				case 0:
					m_proxy_type="None";
					m_proxy_host="";
					m_proxy_port=0;
					break;
				case 1:
					m_proxy_type="Socket 4";
					break;
				case 2:
					m_proxy_type="Socket 5";
					break;
				case 3:
					m_proxy_type="HTTP";
					break;
				case 4:
					m_proxy_type="Telnet";
					break;
				}
			}
			if(type!=0)
			{
				get_arg_line(ch,"ProxyHost",str);
				m_proxy_host=str;
				get_arg_line(ch,"ProxyPort",str);
				m_proxy_port=atoi(str);
			}
		}

	}	
	free(buf);
}


void Setting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(Setting)
	DDX_Text(pDX, IDC_HOST1, m_host1);
	DDV_MaxChars(pDX, m_host1, 64);
	DDX_Text(pDX, IDC_HOST2, m_host2);
	DDV_MaxChars(pDX, m_host2, 64);
	DDX_Text(pDX, IDC_HOST3, m_host3);
	DDV_MaxChars(pDX, m_host3, 64);
	DDX_Text(pDX, IDC_HOST4, m_host4);
	DDV_MaxChars(pDX, m_host4, 64);
	DDX_Text(pDX, IDC_LINES1, m_lines1);
	DDV_MinMaxInt(pDX, m_lines1, 15, 80);
	DDX_Text(pDX, IDC_LINES2, m_lines2);
	DDV_MinMaxInt(pDX, m_lines2, 15, 80);
	DDX_Text(pDX, IDC_LINES3, m_lines3);
	DDV_MinMaxInt(pDX, m_lines3, 15, 80);
	DDX_Text(pDX, IDC_LINES4, m_lines4);
	DDV_MinMaxInt(pDX, m_lines4, 15, 80);
	DDX_CBString(pDX, IDC_PROTOCOL1, m_prot1);
	DDV_MaxChars(pDX, m_prot1, 32);
	DDX_CBString(pDX, IDC_PROTOCOL2, m_prot2);
	DDV_MaxChars(pDX, m_prot2, 32);
	DDX_CBString(pDX, IDC_PROTOCOL3, m_prot3);
	DDV_MaxChars(pDX, m_prot3, 32);
	DDX_CBString(pDX, IDC_PROTOCOL4, m_prot4);
	DDV_MaxChars(pDX, m_prot4, 32);
	DDX_CBString(pDX, IDC_TERMINALLIST1, m_term1);
	DDV_MaxChars(pDX, m_term1, 32);
	DDX_CBString(pDX, IDC_TERMINALLIST2, m_term2);
	DDV_MaxChars(pDX, m_term2, 32);
	DDX_CBString(pDX, IDC_TERMINALLIST3, m_term3);
	DDV_MaxChars(pDX, m_term3, 32);
	DDX_CBString(pDX, IDC_TERMINALLIST4, m_term4);
	DDV_MaxChars(pDX, m_term4, 32);
	DDX_Text(pDX, IDC_PORT1, m_port1);
	DDX_Text(pDX, IDC_PORT2, m_port2);
	DDX_Text(pDX, IDC_PORT3, m_port3);
	DDX_Text(pDX, IDC_PORT4, m_port4);
	DDX_CBString(pDX, IDC_PROXY, m_proxy_type);
	DDX_Text(pDX, IDC_PROXY_HOST, m_proxy_host);
	DDX_Text(pDX, IDC_PROXY_PORT, m_proxy_port);
	DDX_Radio(pDX, IDC_RADIO1, m_act);
	DDX_CBString(pDX, IDC_CHARSET1, m_charset1);
	DDX_CBString(pDX, IDC_CHARSET2, m_charset2);
	DDX_CBString(pDX, IDC_CHARSET3, m_charset3);
	DDX_CBString(pDX, IDC_CHARSET4, m_charset4);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(Setting, CDialog)
	//{{AFX_MSG_MAP(Setting)
	ON_BN_CLICKED(IDCANCEL2, OnCancel2)
	ON_BN_CLICKED(IDC_CONNECT, OnConnect)
	ON_BN_CLICKED(IDOK2, OnSave)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Setting message handlers

void Setting::OnSave() 
{
	// TODO: Add your control notification handler code here
	if (!UpdateData(TRUE))
	{
		AfxMessageBox("Dialog Error!");
		return;
	}
	int fd=_creat(path+"etc/terminal.host",_S_IREAD | _S_IWRITE);

	char str[1024];

	sprintf(str,"Host=%s Prot=%s Port=%d Term=%s Lines=%d Charset=%s\r\n",m_host1,m_prot1,m_port1,m_term1,m_lines1,m_charset1);
	write(fd,str,strlen(str));
	sprintf(str,"Host=%s Prot=%s Port=%d Term=%s Lines=%d Charset=%s\r\n",m_host2,m_prot2,m_port2,m_term2,m_lines2,m_charset2);
	write(fd,str,strlen(str));
	sprintf(str,"Host=%s Prot=%s Port=%d Term=%s Lines=%d Charset=%s\r\n",m_host3,m_prot3,m_port3,m_term3,m_lines3,m_charset3);
	write(fd,str,strlen(str));
	sprintf(str,"Host=%s Prot=%s Port=%d Term=%s Lines=%d Charset=%s\r\n",m_host4,m_prot4,m_port4,m_term4,m_lines4,m_charset4);
	write(fd,str,strlen(str));

	sprintf(str,"ProxyHost=%s ProxyPort=%d ProxyType=%d\r\n",m_proxy_host,m_proxy_port,
		!strcmp(m_proxy_type,"Socket 4")?1:
		!strcmp(m_proxy_type,"Socket 5")?2:
		!strcmp(m_proxy_type,"HTTP")?3:
		!strcmp(m_proxy_type,"Telnet")?4:0);
	write(fd,str,strlen(str));

	close(fd);	
	EndDialog(true);	
}

void Setting::OnCancel2() 
{
	// TODO: Add your control notification handler code here
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



void Setting::OnConnect() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	*act=m_act;
	OnSave();
}
