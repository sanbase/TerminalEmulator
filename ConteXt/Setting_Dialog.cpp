// Setting_Dialog.cpp : implementation file
//

#include "stdafx.h"
#include "ConteXt.h"
#include "Setting_Dialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include <io.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
/////////////////////////////////////////////////////////////////////////////
// Setting_Dialog dialog

char *get_str(char *,int);
Setting_Dialog::Setting_Dialog(CWnd* pParent /*=NULL*/)
	: CDialog(Setting_Dialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(Setting_Dialog)
	m_term = _T("32");
	m_lines = 0;
	//}}AFX_DATA_INIT
	struct stat st;
	char *buf=NULL;
	int len;
	CString host;
	int fd=open("C:/Program Files/UnixSpace/etc/cx5.set",_O_RDONLY|_O_BINARY);
	if(fd>0)
	{
		fstat(fd,&st);
		len=st.st_size;
		buf=(char *)calloc(len+1,1);
		read(fd,buf,len);
		close(fd);
		char *ch=get_str(buf,0);
		if(ch!=NULL)
			host=ch;
		else host="Host=127.0 Prot=CX5 Term=UnixSpace Lines=32";
	}
	else host="Host=127.0 Prot=CX5 Term=UnixSpace Lines=32";
	
	char t_host[256];
	char term[256];
	char prot[256];

    if(sscanf(host,"Host=%s Prot=%s Term=%s Lines=%d\n",t_host,prot,term,&m_lines)==4)
	{
		m_term=term;
	}
	free(buf);
}


void Setting_Dialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(Setting_Dialog)
	DDX_CBString(pDX, IDC_COMBO1, m_term);
	DDX_Text(pDX, IDC_EDIT1, m_lines);
	DDV_MinMaxInt(pDX, m_lines, 20, 60);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(Setting_Dialog, CDialog)
	//{{AFX_MSG_MAP(Setting_Dialog)
	ON_BN_CLICKED(IDOK, OnSave)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Setting_Dialog message handlers

void Setting_Dialog::OnSave() 
{
	// TODO: Add your control notification handler code here
	if (!UpdateData(TRUE))
	{
		AfxMessageBox("Dialog Error!");
		return;
	}

	char str[256];
	sprintf(str,"Host=127.1 Prot=CX5 Term=%s Lines=%d\n",m_term,m_lines);

	struct stat st;
	char *buf=NULL;
	int len;
	int fd=_open("C:/Program Files/UnixSpace/etc/cx5.set",_O_RDONLY|_O_BINARY);
	if(fd>0)
	{
		fstat(fd,&st);
		len=st.st_size;
		buf=(char *)calloc(len+1,1);
		read(fd,buf,len);
		close(fd);
	}
	fd=_creat("C:/Program Files/UnixSpace/etc/cx5.set",_S_IREAD | _S_IWRITE);
	for(int i=0;i<4;i++)
	{
		char *ch=get_str(buf,i);
		if(i==0)
		{
			write(fd,str,strlen(str));
		}
		else
		{
			if(ch!=NULL)
				write(fd,ch,strlen(ch));
		}
		write(fd,"\n",1);
	}
	close(fd);	
	EndDialog(true);
	free(buf);	
}
