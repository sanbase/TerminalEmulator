// ConteXtView.cpp : implementation of the CConteXtView class
//

#include "stdafx.h"
#include "ConteXt.h"

#include "ConteXtDoc.h"
#include "ConteXtView.h"

#include <io.h>
#include <sys\stat.h>
#include <fcntl.h>
#include <afxinet.h>

#include "../terminalView.h"
#include "../TBitmap.h"
#include "../channel.h"
#include "../Display.h"
#include "../telnetIO.h"  
#include "../CImage.h"
#include "../object.h"
#include "Registration.h"
#include "Setting_Dialog.h"

extern int current_screen;
void __cdecl start_scene(void* scene);

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//#define CX5_TEST 1
extern long GetVSNumber();
/////////////////////////////////////////////////////////////////////////////
// CConteXtView

IMPLEMENT_DYNCREATE(CConteXtView, CTerminalView)

BEGIN_MESSAGE_MAP(CConteXtView, CTerminalView)
	//{{AFX_MSG_MAP(CConteXtView)
	ON_COMMAND(ID_CONNECT, OnConnect)
	ON_COMMAND(ID_SETUP, OnSetup)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConteXtView construction/destruction

CConteXtView::CConteXtView():CTerminalView()
{
	// TODO: add construction code here
}

CConteXtView::~CConteXtView()
{
}

BOOL CConteXtView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CTerminalView::PreCreateWindow(cs);
}

void CConteXtView::init()
{
	act=0;
	current_screen=0;
	num_channels=1;
	m_channel=(channel **)calloc(num_channels,sizeof (channel **));
	m_channel[act]=new channel(this,0);
	pressedKey=' ';
	Connect();
	m_StopThread=false;
	m_Thread = _beginthread(start_scene, 0, this);
	SetThreadPriority((void*)m_Thread, THREAD_PRIORITY_LOWEST);
}
/////////////////////////////////////////////////////////////////////////////
// CConteXtView drawing


void CConteXtView::OnDraw(CDC* pDC)
{
	CConteXtDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	OnPrepareDC(pDC);

	if(m_channel==NULL)
		init();
	m_channel[act]->update[0]=true;
//	CTerminalView::OnDraw(pDC);

}
char *get_str(char *,int);
void CConteXtView::Connect() 
{
	// TODO: Add your command handler code here
	if(m_channel[act]!=NULL)
		m_channel[act]->Disconnect();

	int fd=open("C:/Program Files/UnixSpace/etc/cx5.set",_O_RDONLY);
	if(fd>0)
	{
		char str[256];
      
		memset(str,0,sizeof str);
		read(fd,str,sizeof str);
		close(fd);

		char *ch=get_str(str,act);
		if(ch!=NULL)
			host[act]=ch;
		else
			host[act]="Host=127.1 Prot=CX5 Term=UnixSpace Lines=32\n";
	}
	else
	{
		int lines;
		CRect rec;
		GetClientRect(&rec);
		lines=rec.Height()/23;
		char str[256];
		sprintf(str,"Host=127.1 Prot=CX5 Term=UnixSpace Lines=%d\n",lines);
		host[act]=str;
		fd=_creat("C:/Program Files/UnixSpace/etc/cx5.set",_S_IREAD | _S_IWRITE);
		write(fd,str,strlen(str));
		close(fd);
//		host[act]="Host=127.1 Port=8177 Lines=32 Term=UnixSpace\n";
	}
	OnConteXtBrowser();
}

/////////////////////////////////////////////////////////////////////////////
// CConteXtView diagnostics
#ifdef _DEBUG
void CConteXtView::AssertValid() const
{
	CTerminalView::AssertValid();
}

void CConteXtView::Dump(CDumpContext& dc) const
{
	CTerminalView::Dump(dc);
}

CConteXtDoc* CConteXtView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CConteXtDoc)));
	return (CConteXtDoc*)m_pDocument;
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CConteXtView message handlers

void CConteXtView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();
	// TODO: Add your specialized code here and/or call the base class

	int fd,hi,lo,key,x;
	DWORD k;	
	char *buf[256];
	struct stat st;
	if(!stat("C:/Program Files/UnixSpace/bin/cx5.exe",&st))
	{
		if((fd=open("C:/Program Files/UnixSpace/etc/CX5.key",O_RDONLY))<0)                                         
			goto ASK;
	}
	else
	{
		AfxMessageBox("The file cx5.exe should be in the C:\\Program Files\\UnixSpace\\bin directory.");
		exit(0);
	}
	memset(buf,0,sizeof buf);
	read(fd,buf,sizeof buf);
	close(fd);
	key=atoi((const char *)buf);
	k=st.st_ctime+GetVSNumber(); 
	x=k%673452;
	hi = x / 127773;
	lo = x % 127773;
	x = 16807 * lo - 2836 * hi;
	if (x <= 0)
		x += 0x7fffffff;
	x%=100000;
	if(key==x)
		return;
ASK:
	Registration rg;
	rg.DoModal();	
}

void CConteXtView::OnConteXtBrowser() 
{
	// TODO: Add your command handler code here
	if(access("C:/Program Files/UnixSpace/bin/cx5.exe",0))
	{
		AfxMessageBox("The ConteXt Database Browser do not installed.\nVisit www.UnixSpace.com/Download");
		exit(0);
	}
	if(m_channel[act]!=NULL)
	{
		m_channel[act]->Disconnect();
	}
	else m_channel[act]=new channel(this,act);

	if(Terminal_PID!=0)
		TerminateProcess(Terminal_PID,0);

	STARTUPINFO StartInfo;
	PROCESS_INFORMATION PInfo;
	memset(&StartInfo,0,sizeof StartInfo);

#ifndef CX5_TEST
	if(CreateProcess("C:/Program Files/UnixSpace/bin/cx5.exe", NULL,NULL,NULL, false, NULL, NULL, NULL, &StartInfo, &PInfo))
	{
#endif
		Sleep(100);
		m_channel[act]->Connect(host[act]);
		if(m_channel[act]->io->connected==false)
		{
CX_ERROR:
			AfxMessageBox("ConteXt server is not responding");
			TerminateProcess(PInfo.hProcess,0);
			return;
		}
#ifndef CX5_TEST
		Terminal_PID[act]=PInfo.hProcess;
	}
	else	goto CX_ERROR;
#else
		Terminal_PID=NULL;
#endif
	m_channel[act]->update[0]=true;
	m_channel[act]->set_active(true);	
	GetDocument()->SetTitle("Terminal DBMS");
}

void CConteXtView::OnConnect() 
{
	// TODO: Add your command handler code here
	Connect();	
}

void CConteXtView::OnSetup() 
{
	// TODO: Add your command handler code here
		Setting_Dialog Host;
		Host.DoModal();
}
