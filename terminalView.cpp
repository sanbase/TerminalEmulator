// terminalView.cpp : implementation of the CTerminalView class
//

#include "stdafx.h"
#include "terminal.h"

#include "terminalDoc.h"
#include "terminalView.h"
#include "TBitmap.h"
#include "channel.h"
#include <io.h>
#include <sys\stat.h>
#include <fcntl.h>
#include "Display.h"
#include <afxinet.h>
#include "telnetIO.h"
#include "CImage.h"
#include "object.h"
#include "Registration.h"
#ifndef CX5
//#include "OpenGL.h"
#include "Setting.h"
#include ".\terminalview.h"
#else
#include "ConteXt/Setting_Dialog.h"
#endif
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//#define PROXY_DEBUG 1
/////////////////////////////////////////////////////////////////////////////
// CTerminalView

IMPLEMENT_DYNCREATE(CTerminalView, CView)

BEGIN_MESSAGE_MAP(CTerminalView, CView)
	//{{AFX_MSG_MAP(CTerminalView)
	ON_WM_KEYDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_SYSKEYDOWN()
	ON_WM_CHAR()
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(ID_CONNECT, OnConnect)
	ON_WM_MOUSEWHEEL()
	ON_COMMAND(ID_SCREEN1, OnScreen1)
	ON_COMMAND(ID_SCREEN2, OnScreen2)
	ON_COMMAND(ID_SCREEN3, OnScreen3)
	ON_COMMAND(ID_SCREEN4, OnScreen4)
	ON_COMMAND(ID_HOSTSETTING, OnHostsetting)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
//	ON_WM_PAINT()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTerminalView construction/destruction

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

CTerminalView::CTerminalView()
{
	// TODO: add construction code here
	m_channel=NULL;
    Terminal_PID[0]=NULL;
    Terminal_PID[1]=NULL;
    Terminal_PID[2]=NULL;
    Terminal_PID[3]=NULL;
	visual_eff.eff=0;
	stored=NULL;
	m_gl=NULL;
	HKEY Key;
	LONG Result;
	char tmp[256];
	memset(tmp,0,sizeof tmp);
	LONG len;
	DWORD type;
	path="C:/Program Files/UnixSpace/";
#ifdef CX5
	Result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\UnixSpace\\ConteXt\\" ,0, KEY_READ, &Key);  
#else
	Result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\UnixSpace\\UnixSpace Terminal\\" ,0, KEY_READ, &Key);  
#endif
	if (Result == ERROR_SUCCESS)
	{
		len=sizeof tmp -1;	
        Result = RegQueryValueEx(Key,"InstallPath",NULL,&type,(BYTE *)tmp, (LPDWORD)&len);
	    if (Result == ERROR_SUCCESS)
		{
			path=tmp;
		}
		RegCloseKey(Key);            
	}

	InitializeCriticalSection(&m_DrawSynchro);
}

extern ImageHash *image_hash;
extern int num_images;
int current_screen;
int get_arg_line(CString line,char *pattern,char *arg);
void clean_var();

void CTerminalView::cancel()
{
	m_StopThread = true;
//hile (m_StopThread)
//Sleep(20);
	Sleep(100);
	clean_var();
	DeleteCriticalSection(&m_DrawSynchro);
	for(int i=0;i<num_channels;i++)
	{
		if(m_channel[i]!=NULL)
		{
			delete(m_channel[i]);
		}
	}
	free(m_channel);

	for(i=0;i<num_images;i++)
	{
		if(image_hash[i].image!=NULL)
			delete image_hash[i].image;
		delete image_hash[i].name;
	}
	if(image_hash!=NULL)
		free(image_hash);
}

CTerminalView::~CTerminalView()
{
	for(int i=0;i<4;i++)
	{
		if(Terminal_PID[i]!=NULL)
			TerminateProcess(Terminal_PID[i],0);
	}
	if(stored!=NULL)
		delete stored;
	cancel();
}

void __cdecl start_scene(void* scene) 
{
	(static_cast<CTerminalView*>(scene))->Run();
    _endthread();
}

void __cdecl start_paint(void* scene) 
{
	(static_cast<CTerminalView*>(scene))->mPaint();
    _endthread();
}

void CTerminalView::mPaint()
{
	while(!m_StopThread)		
	{
		Sleep(40); // 25 cadrs per second
		isPaint=1;
	}
}

void CTerminalView::Run()
{
	while (!m_StopThread)
	{
		if(isPaint || stored!=NULL)
		{
			paint();
			isPaint=0;
		}
		Sleep(4);
	}
}

void CTerminalView::init()
{

//	SetUpDrawingEnvironment();		

	act=0;
	current_screen=0;
	num_channels=4;
	m_channel=(channel **)calloc(num_channels,sizeof (channel **));
	m_channel[act]=new channel(this,0);
	pressedKey=' ';

	Connect();
	m_StopThread=false;

	_beginthread(start_paint, 0, this);
	m_Thread = _beginthread(start_scene, 0, this);
	SetThreadPriority((void*)m_Thread, THREAD_PRIORITY_LOWEST);
}

BOOL CTerminalView::PreCreateWindow(CREATESTRUCT& cs)
{
  // necessary in all OpenGL-windows:
	cs.style|=WS_CLIPSIBLINGS|WS_CLIPCHILDREN;

	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	return(CView::PreCreateWindow(cs));
}

/////////////////////////////////////////////////////////////////////////////
// CTerminalView drawing

void CTerminalView::OnDraw(CDC* pDC)
{

/*
	CTerminalDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	OnPrepareDC(pDC);
*/
	if(m_channel==NULL)
		init();
	m_channel[act]->update[0]=true;

}

/////////////////////////////////////////////////////////////////////////////
// CTerminalView diagnostics

#ifdef _DEBUG
void CTerminalView::AssertValid() const
{
	CView::AssertValid();
}

void CTerminalView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CTerminalDoc* CTerminalView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTerminalDoc)));
	return (CTerminalDoc*)m_pDocument;
}

#endif //_DEBUG


void CTerminalView::set_active(int i)
{
        if(i==act)
		{
			m_channel[act]->update[0]=true;
            return;
		}
        m_channel[act]->set_active(false);
//        Change_Act_Item(act,i);
        if(m_channel[i]==NULL)
        {
				act=i;
				current_screen=act;
                m_channel[i]=new channel(this,act);
				Connect();

//                m_channel[i]->Connect(host);
        }
        act=i;
		current_screen=act;
        m_channel[act]->set_active(true);

		char str[256],str1[256];
		*str=0;
		get_arg_line(host[act],"Host",str);
		sprintf(str1,"Screen %d: %s",act+1,str);
		GetDocument()->SetTitle(str1);
        m_channel[act]->update[0]=true;
//		paint();
}

//#define VISUAL_EFF

#define PIXEL(buf,x,y) buf[x+y*bmi.bmiHeader.biWidth]

void  CTerminalView::Visual_Eff_Exec(int a,int i,int x, int y, int l, int h)
{
	if(i==0)
	{
		if(a!=0)
		{
			if(stored!=NULL)
				delete stored;
			stored=NULL;
			return;
		}
		if(stored==NULL)
			stored = new RGBQUAD [m_channel[act]->term->pBitmap->Width*m_channel[act]->term->pBitmap->Height*sizeof (RGBQUAD)];
		memcpy(stored,m_channel[act]->term->pBitmap->buffer,m_channel[act]->term->pBitmap->Width*m_channel[act]->term->pBitmap->Height*sizeof (RGBQUAD));
	}
	if(stored!=NULL)
	{
		visual_eff.eff=a;
		visual_eff.n=i;
		visual_eff.x=x*m_channel[act]->term->charWidth;
		visual_eff.y=y*m_channel[act]->term->charHeight;
		visual_eff.l=(x+l)*m_channel[act]->term->charWidth;
		visual_eff.h=(y+h)*m_channel[act]->term->charHeight;
		if(visual_eff.l==visual_eff.x)
			visual_eff.l=m_channel[act]->term->pBitmap->Width;
		if(visual_eff.h==visual_eff.y)
			visual_eff.h=m_channel[act]->term->pBitmap->Height;
		scale=i;
	}
}

void CTerminalView::VisualEff(CDC *pDC)
{
	if(stored==NULL)
		return;

	BITMAPINFO bmi;

	bmi.bmiHeader.biSize   = sizeof bmi.bmiHeader;
	bmi.bmiHeader.biWidth  = m_channel[act]->term->pBitmap->Width;
	bmi.bmiHeader.biHeight = m_channel[act]->term->pBitmap->Height;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biSizeImage = 0;
	
	RGBQUAD *buffer;
	HBITMAP hBitmap=NULL;
	CBitmap bmp;
	CDC *m_dc=NULL;	
	m_dc = new CDC();
	m_dc->CreateCompatibleDC(NULL);

	hBitmap = CreateDIBSection(m_channel[act]->term->pBitmap->Canvas->m_hDC, &bmi, DIB_RGB_COLORS,(void**)&buffer, 0, 0);
	bmp.Attach(hBitmap);
	m_dc->SelectObject(&bmp);

//		m_dc->BitBlt(2,0,bmi.bmiHeader.biWidth,bmi.bmiHeader.biHeight,m_channel[act]->term->pBitmap->Canvas,0,0,SRCCOPY);

	register RGBQUAD *i_buf = m_channel[act]->term->pBitmap->buffer;
	register RGBQUAD *o_buf = buffer; 
	register RGBQUAD *s_buf = stored; 
	int x,y,k;

	if(visual_eff.n!=0)
		k=(visual_eff.n<<8)/scale;
	switch(visual_eff.eff)
	{
	case 1:
		for(y=bmi.bmiHeader.biHeight-1;y>=0;y--)
		{			
			for(x=0;x<bmi.bmiHeader.biWidth;x++)
			{
				if(x>visual_eff.x && x<visual_eff.l && y>visual_eff.y && y<visual_eff.h)
				{	
					if(visual_eff.n!=0 && (*(int *)s_buf!=*(int *)i_buf))
					{
						o_buf->rgbRed  = (int)(i_buf->rgbRed  +(((s_buf->rgbRed  -i_buf->rgbRed)  *k)>>8));
						o_buf->rgbGreen= (int)(i_buf->rgbGreen+(((s_buf->rgbGreen-i_buf->rgbGreen)*k)>>8));
						o_buf->rgbBlue = (int)(i_buf->rgbBlue +(((s_buf->rgbBlue -i_buf->rgbBlue) *k)>>8));
					}
					else	
						*o_buf=*s_buf;
				}
				else	
					*o_buf=*i_buf;
				o_buf++;
				i_buf++;
				s_buf++;
			}
		}
		break;
	case 2:
		for(y=bmi.bmiHeader.biHeight-1;y>=0;y--)
		{
			if(y>visual_eff.y && y<visual_eff.h)
			{
				int a=y-visual_eff.y>(visual_eff.n*(visual_eff.h-visual_eff.y))/scale;
				if(a)
					memcpy(o_buf+visual_eff.x,i_buf+visual_eff.x,(visual_eff.l-visual_eff.x)*sizeof (RGBQUAD));
				else
					memcpy(o_buf+visual_eff.x,s_buf+visual_eff.x,(visual_eff.l-visual_eff.x)*sizeof (RGBQUAD));
			}
			else
				memcpy(o_buf,i_buf,bmi.bmiHeader.biWidth*sizeof (RGBQUAD));
			o_buf+=bmi.bmiHeader.biWidth;
			i_buf+=bmi.bmiHeader.biWidth;
			s_buf+=bmi.bmiHeader.biWidth;
		}
		break;
	case 3:
		for(y=bmi.bmiHeader.biHeight-1;y>=0;y--)
		{
			k=0;
			for(x=0;x<bmi.bmiHeader.biWidth;x++,k++)
			{
				if(x>visual_eff.x && x<visual_eff.l && y>visual_eff.y && y<visual_eff.h)
				{
					if(visual_eff.n!=0)
					{
						if((k%scale)>=visual_eff.n)
							*o_buf=*i_buf;
						else *o_buf=*s_buf;
					}
					else	
						*o_buf=*s_buf;
				}
				else	
					*o_buf=*i_buf;
				o_buf++;
				i_buf++;
				s_buf++;
			}
		}
		break;
	case 4:
		k=visual_eff.x+((visual_eff.l-visual_eff.x)*(scale-visual_eff.n))/scale;
		for(y=bmi.bmiHeader.biHeight-1;y>=0;y--)
		{
			for(x=0;x<bmi.bmiHeader.biWidth;x++)
			{
				if(x>visual_eff.x && x<visual_eff.l && y>visual_eff.y && y<visual_eff.h)
				{
					if(visual_eff.n!=0)
					{
						if(x<=k)
							*o_buf=*i_buf;
						else 
							*o_buf=*s_buf;
					}
					else	
						*o_buf=*s_buf;
				}
				else	
					*o_buf=*i_buf;
				o_buf++;
				i_buf++;
				s_buf++;
			}
		}
		break;
	default:
		visual_eff.n=1;
		break;
	}
	if(visual_eff.n!=0)
	{
		if((--visual_eff.n)==0)
		{
			delete stored;
			stored=NULL;			
			pDC->BitBlt(2,0,bmi.bmiHeader.biWidth,bmi.bmiHeader.biHeight,m_channel[act]->term->pBitmap->Canvas,0,0,SRCCOPY);	
			goto END;
		}
	}

	pDC->BitBlt(2,0,bmi.bmiHeader.biWidth,bmi.bmiHeader.biHeight,m_dc,0,0,SRCCOPY);

END:
	if(hBitmap!=NULL)
		DeleteObject(hBitmap);
	if(m_dc!=NULL)
		delete m_dc;
}

void flipIt(void* buffer,int len) 
{
  void* b = buffer;     
__asm                  
  {
    mov ecx, len    

    mov ebx, b          

    label:              
      mov al,[ebx+0]    
      mov ah,[ebx+2]    
      mov [ebx+2],al    
      mov [ebx+0],ah    

      add ebx,3         
      dec ecx           

	jnz label             
  }
}

void CTerminalView::paint()
{
	if(m_channel==NULL || m_channel[act]==NULL || m_channel[act]->term==NULL)
		return;
	EnterCriticalSection(&m_DrawSynchro);

	m_channel[act]->term->paint();
	CDC *pDC = GetDC();
	if(stored!=NULL)
	{
		VisualEff(pDC);
	}
	else if(m_channel[act]->update[0])
	{
		pDC->BitBlt(2,0,m_channel[act]->term->pBitmap->Width,m_channel[act]->term->pBitmap->Height,m_channel[act]->term->pBitmap->Canvas,0,0,SRCCOPY);
	}
	else
	{
		CRect tr;
		GetClientRect(&tr);
		for(int i=0;i<m_channel[act]->term->pBitmap->Height;i++)
		{
			if(m_channel[act]->update[i+1])
			{
				int j=i;
				for(;i<m_channel[act]->term->pBitmap->Height;i++)
				{
					if(m_channel[act]->update[i+1]==false)
						break;
				}
				tr.top=j*m_channel[act]->term->charHeight-1;
				if(tr.top<0)
				    tr.top=0;
				tr.bottom=tr.top+(i-j+1)*m_channel[act]->term->charHeight+2;
				pDC->BitBlt(tr.left+2,tr.top,tr.Width()-4,tr.Height(),m_channel[act]->term->pBitmap->Canvas,tr.left,tr.top,SRCCOPY);
			}
		}
	}
	ReleaseDC(pDC);

	memset(m_channel[act]->update,0,sizeof m_channel[act]->update);
	LeaveCriticalSection(&m_DrawSynchro);
}

/////////////////////////////////////////////////////////////////////////////
// CTerminalView message handlers

void CTerminalView::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	int i=-1;
	char *ch;
	switch(nChar)
	{
		case VK_F10:
            if((ch=m_channel[act]->Output_Code(9))!=NULL)
				m_channel[act]->Write(ch);
			else
				m_channel[act]->Write("\x1b[21~");
			pressedKey=0;
			return;
		case 'Z':
            if((ch=m_channel[act]->Output_Code(11))!=NULL)
				m_channel[act]->Write(ch);
			else
				m_channel[act]->Write("\x1b[24~");
				break;
		case 'A':
				m_channel[act]->term->rus=8;
				break;
		case 'S':
				m_channel[act]->term->rus=866;
				break;
		case 'D':
				m_channel[act]->term->rus=0;
				break;
/*
#ifndef CX5
		case VK_F1:
		{
			if(m_gl!=NULL)
				delete m_gl;
			m_gl=new COpenGL();

			int i;
			CRect rc,wnd;
			GetClientRect(&wnd);
			rc=wnd;
			for(i=1;;i*=2)
			{
				if(i>rc.bottom)
				{
					rc.bottom=i/2;
					break;
				}
			}
			for(i=1;;i*=2)
			{
				if(i>rc.right)
				{
					rc.right=i/2;
					break;
				}
			}
			rc.left=(wnd.right-rc.right)/2;
			rc.top=(wnd.bottom-rc.bottom)/2;
			rc.right+=rc.left;
			rc.bottom+=rc.top;
			m_gl->Create( NULL,   //CWnd default
						NULL,   //has no name
						WS_CHILD|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|WS_VISIBLE,
						rc,
						this,   //this is the parent
						0);     //this should really be a different number... check resource.h
			m_gl->ShowWindow(SW_SHOW);
			break;
		}
		case VK_F2:
			if(m_gl!=NULL)
				delete m_gl;
			m_gl=NULL;
			break;
#endif
*/
	}
//	CView::OnSysKeyDown(nChar, nRepCnt, nFlags);
}

void CTerminalView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default

	if(m_channel[act]->io->connected==false)
	{
		Connect();
//		CView::OnKeyDown(nChar, nRepCnt, nFlags);
		return;
	}
    char *ch; 
	switch(nChar)
	{
		case VK_F1:
			if((ch=m_channel[act]->Output_Code(0))!=NULL)
				m_channel[act]->Write(ch);
			else
				m_channel[act]->Write("\x1bOP");
			break;
		case VK_F2:
			if((ch=m_channel[act]->Output_Code(1))!=NULL)
				m_channel[act]->Write(ch);
			else
				m_channel[act]->Write("\x1bOQ");
			break;
		case VK_F3:
			if((ch=m_channel[act]->Output_Code(2))!=NULL)
				m_channel[act]->Write(ch);
			else
				m_channel[act]->Write("\x1bOR");
			break;
		case VK_F4:
            if((ch=m_channel[act]->Output_Code(3))!=NULL)
				m_channel[act]->Write(ch);
			else
			    m_channel[act]->Write("\x1bOS");
			break;
		case VK_F5:
            if((ch=m_channel[act]->Output_Code(4))!=NULL)
				m_channel[act]->Write(ch);
			else
				m_channel[act]->Write("\x1b[15~");
			break;
		case VK_F6:
            if((ch=m_channel[act]->Output_Code(5))!=NULL)
				m_channel[act]->Write(ch);
			else
				m_channel[act]->Write("\x1b[17~");
			break;
		case VK_F7:
            if((ch=m_channel[act]->Output_Code(6))!=NULL)
				m_channel[act]->Write(ch);
			else
				m_channel[act]->Write("\x1b[18~");
			break;
		case VK_F8:
            if((ch=m_channel[act]->Output_Code(7))!=NULL)
				m_channel[act]->Write(ch);
			else
				m_channel[act]->Write("\x1b[19~");
			break;
		case VK_F9:
			if((ch=m_channel[act]->Output_Code(8))!=NULL)
				m_channel[act]->Write(ch);
			else
				m_channel[act]->Write("\x1b[20~");
			break;
		case VK_ESCAPE:
		case VK_F10:
            if((ch=m_channel[act]->Output_Code(9))!=NULL)
				m_channel[act]->Write(ch);
			else
				m_channel[act]->Write("\x1b[21~");
			break;
		case VK_F11:
			if((ch=m_channel[act]->Output_Code(10))!=NULL)
				m_channel[act]->Write(ch);
			else
				m_channel[act]->Write("\x1b[23~");
			break;
		case VK_F12:
            if((ch=m_channel[act]->Output_Code(11))!=NULL)
				m_channel[act]->Write(ch);
			else
				m_channel[act]->Write("\x1b[24~");
			break;
		case VK_UP:
            if((ch=m_channel[act]->Output_Code(12))!=NULL)
				m_channel[act]->Write(ch);
			else
				m_channel[act]->Write("\x1b[A");
			break;
		case VK_DOWN:
			if((ch=m_channel[act]->Output_Code(13))!=NULL)
				m_channel[act]->Write(ch);
			else
				m_channel[act]->Write("\x1b[B");
			break;
		case VK_LEFT:
            if((ch=m_channel[act]->Output_Code(14))!=NULL)
				m_channel[act]->Write(ch);
			else
				m_channel[act]->Write("\x1b[D");
			break;
		case VK_RIGHT:
			if((ch=m_channel[act]->Output_Code(15))!=NULL)
				m_channel[act]->Write(ch);
			else
				m_channel[act]->Write("\x1b[C");
			break;
		case VK_HOME:
			if((ch=m_channel[act]->Output_Code(16))!=NULL)
				m_channel[act]->Write(ch);
			else
				m_channel[act]->Write("\x1b[7~");
			break;
		case VK_END:
			if((ch=m_channel[act]->Output_Code(17))!=NULL)
				m_channel[act]->Write(ch);
			else
				m_channel[act]->Write("\x1b[8~");
			break;
		case VK_DELETE:
			if((ch=m_channel[act]->Output_Code(18))!=NULL)
				m_channel[act]->Write(ch);
			else
				m_channel[act]->Write("\x7f");
			break;
		case VK_PRIOR:
			if((ch=m_channel[act]->Output_Code(19))!=NULL)
				m_channel[act]->Write(ch);
			else
				m_channel[act]->Write("\x1b[5~");
			break;
		case VK_NEXT:
			if((ch=m_channel[act]->Output_Code(20))!=NULL)
				m_channel[act]->Write(ch);
			else
				m_channel[act]->Write("\x1b[6~");
			break;
		case VK_INSERT:
			if((ch=m_channel[act]->Output_Code(21))!=NULL)
				m_channel[act]->Write(ch);
			else
				m_channel[act]->Write("\x1b[2~");
			break;
		default:
			CView::OnKeyDown(nChar, nRepCnt, nFlags);
			return;
	}
	pressedKey=0;
//	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CTerminalView::OnMouseMove(UINT nFlags, CPoint point) 
{
	if(m_channel==NULL)
		return;
	if(m_channel[act]->io->connected==true)
	{
		m_channel[act]->term->check_coord(point.x,point.y,0);
		if(nFlags==MK_LBUTTON)
		{
			m_channel[act]->term->m_end=m_channel[act]->term->get_coord(point.x,point.y);
			m_channel[act]->term->update[0]=true;
		}
	}
}

void CTerminalView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if(m_channel[act]->io->connected==true)
	{
		char str[64];
		POINT xy=m_channel[act]->term->image_coord(point.x,point.y);
                if(xy.x==-1 || xy.y==-1)
		xy = m_channel[act]->term->get_coord(point.x,point.y);
		if(m_channel[act]->term->m_start!=m_channel[act]->term->m_end)
			m_channel[act]->term->update[0]=true;
		m_channel[act]->term->m_start=xy;
		m_channel[act]->term->m_end=xy;
		if(!strcmp(m_channel[act]->term_type,"UnixSpace"))
		{
			sprintf(str,"%c%%%d;%d;1H",27,xy.x,xy.y);
			m_channel[act]->Write(str);
		}
		pressedKey=0;
	}
}

void CTerminalView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	if(m_channel[act]->io->connected==true && !strcmp(m_channel[act]->term_type,"UnixSpace"))
	{
		char str[64];
		POINT xy=m_channel[act]->term->image_coord(point.x,point.y);
		if(xy.x==-1 || xy.y==-1)
			xy = m_channel[act]->term->get_coord(point.x,point.y);
		sprintf(str,"%c%%%d;%d;3H",27,xy.x,xy.y);
		m_channel[act]->Write(str);
		pressedKey=0;
	}		
}

void CTerminalView::OnChar(UINT Key, UINT nRepCnt, UINT nFlags) 
{
	if(m_channel[act]->io->connected==false)
	{
		Connect();
		return;
	}
	if(Key=='\n' && pressedKey=='\r')
		m_channel[act]->Write("\n\n");
	else if(Key==3 && m_channel[act]->term->m_start!=m_channel[act]->term->m_end)
	{
		if(OpenClipboard())
		{
			HGLOBAL clipbuffer;
			char * buffer;

			CString source; 
			for(int y=m_channel[act]->term->m_start.y;y<=m_channel[act]->term->m_end.y;y++)
			{
				unsigned char ch;
				int l=m_channel[act]->term->scr->Width();
				while(l>1 && ((ch=m_channel[act]->term->scr->Char(l-1,y-1).ch)<=' ') || ch>127)
					l--;
				for(int x=1;x<=l;x++)
				{
					ch=m_channel[act]->term->scr->Char(x-1,y-1).ch;
					if(ch<' ')
						ch=' ';
					if(x==1 && y!=m_channel[act]->term->m_start.y)
						source+="\r\n";
					if(m_channel[act]->term->m_start!=m_channel[act]->term->m_end && y>=m_channel[act]->term->m_start.y && y<=m_channel[act]->term->m_end.y)
					{
						if(x>=m_channel[act]->term->m_start.x && x<m_channel[act]->term->m_end.x)
						{
							if(y>=m_channel[act]->term->m_start.y && y<=m_channel[act]->term->m_end.y)
								source+=ch;
						}
						else if(y>m_channel[act]->term->m_start.y && y<=m_channel[act]->term->m_end.y && x<=m_channel[act]->term->m_start.x)
								source+=ch;
						else if(y>=m_channel[act]->term->m_start.y && y<m_channel[act]->term->m_end.y && x>m_channel[act]->term->m_start.x)
								source+=ch;
					}
                    						
				}
			}
			EmptyClipboard();
			clipbuffer = GlobalAlloc(GMEM_DDESHARE, source.GetLength()+1);
			buffer = (char*)GlobalLock(clipbuffer);
			strcpy(buffer, LPCSTR(source));
			GlobalUnlock(clipbuffer);
			SetClipboardData(CF_TEXT,clipbuffer);
			CloseClipboard();
			m_channel[act]->term->m_end=m_channel[act]->term->m_start;
			m_channel[act]->term->update[0]=true;
		}
	}
	else if(Key==22)
	{
		if(OpenClipboard()) 
		{
			//open the clipboard
//			CString fromClipboard;
			HANDLE hData = GetClipboardData( CF_TEXT );
			if(hData==NULL)
			{
				CloseClipboard();
				return;
			}
			char * buffer = (char*)GlobalLock( hData );
//			fromClipboard = buffer;
			m_channel[act]->Write('\n');
			int rn=0;
			for(int i=0;buffer[i];i++)
			{
				if(buffer[i]=='\n')
				{
					if(rn)
					{
						rn=0;
						continue;
					}
					rn=0;
					m_channel[act]->Write('\r');
						continue;
				}
				m_channel[act]->Write(buffer[i]);
				if(buffer[i]=='\r')
						rn=1;
				else	rn=0;
			}
			m_channel[act]->Write('\n');
			GlobalUnlock( hData );
			CloseClipboard();
		}
	}
	else
	{
		CString str; 
		if(Key>127 && Key<256)
		{
			if(m_channel[act]->term->rus==8)
			{
				static int table[]={225,226,247,231,228,229,246,250,233,234,235,236,237,238,239,240,242,243,244,245,230,232,227,254,251,253,223,249,248,252,224,241,193,194,215,199,196,197,214,218,201,202,203,204,205,206,207,208,136,137,138,139,140,141,142,143,144,145,146,147,148,149,150,151,225,226,247,231,228,229,246,250,233,234,235,236,237,238,239,240,242,243,244,245,230,232,227,254,251,253,223,249,248,252,224,241,193,194,215,199,196,197,214,218,201,202,203,204,205,206,207,208,210,211,212,213,198,200,195,222,219,221,223,217,216,220,192,209};
				Key=table[Key-128];
			}
			else if(m_channel[act]->term->rus==866)
			{
                if(Key>=224-32 && Key<224+16)
                        Key+=160-224;
                else
                if(Key>=224+16)
                        Key-=16;
			}
		}
		str+=(char)Key;
		m_channel[act]->Write(str);
	}
	pressedKey=Key;
}

void CTerminalView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	OnLButtonDown(nFlags,point);
}

int get_arg_line(CString line,char *pattern,char *arg) 
{ 
	char *ch=strstr(line,pattern); 
	int flag=0; 
	if(ch==NULL) 
	{ 
		*arg=0;
		return(0); 
	} 
	ch+=strlen(pattern); 
	while(*ch==' ' || *ch=='\t' || *ch=='=') 
		ch++; 
	if(*ch=='"') 
	{ 
		ch++; 
		flag=1; 
	} 
	for(int i=0;ch[i]!=0;i++) 
	{ 
		if(flag==0 && (ch[i]==' ' || ch[i]==',' || ch[i]=='\t')) 
			break; 
		if(flag && ch[i]=='"') 
			break;
		arg[i]=ch[i]; 
		arg[i+1]=0; 
	} 
	return(1); 
} 

char *get_str(char *buf,int num)
{
	if(buf==NULL)
		return(NULL);
	char *beg=buf;
	char *end;
	for(int i=0;i<=num;i++)
	{
		end=strchr(beg,'\r');
		if(end==NULL)
		{
			end=strchr(beg,'\n');
			if(end==NULL)
				return(NULL);
		}
		if(i==num)
			break;
		beg=end+1;
		if(*beg=='\n')
			beg++;
	}
	if(*beg=='\n' || *beg=='\r')
		return(NULL);
	static char str[256];
	memset(str,0,sizeof str);
	memcpy(str,beg,end-beg);
	return(str);
}

void CTerminalView::OnConnect() 
{
	Connect();
}

void CTerminalView::Connect() 
{
	// TODO: Add your command handler code here
#ifndef CX5
	if(m_channel[act]!=NULL)
		m_channel[act]->Disconnect(); 
#endif

#ifndef PROXY_DEBUG
 #ifdef CX5
	host[act]="Host=127.1 Prot=cx5 Port=80 Term=UnixSpace Lines=40\n"; 
 #else
	int fd=open(path+"etc/terminal.host",_O_RDONLY);
	if(fd>0)	
	{
		char *str; 
		struct stat st;
		fstat(fd,&st);
		str=(char *)calloc(st.st_size+2,1);
		read(fd,str,st.st_size);
		close(fd);

		if(str[strlen(str)-1]!='\n')
			strcat(str,"\n");
		char *ch=get_str(str,act);
		if(ch!=NULL)
			host[act]=ch;
		else
			host[act]="Host=ehhs.cmich.edu Prot=telnet Port=23 Term=VT220 Lines=40\n";
		ch=get_str(str,4);
		if(ch!=NULL)
		{
			host[act]+=" ";
			host[act]+=ch;
		}
		free(str);		
		char host_name[64];
		
        if(get_arg_line(host[act],"Host",host_name)==0)
		{
			unlink(path+"etc/terminal.host");	
			goto RUN_DIALOG;
		}
		CTerminalDoc* pDoc = GetDocument();
		if(pDoc!=NULL)
		{
			char str1[256];
			sprintf(str1,"Screen %d: %s",act+1,host_name);
			pDoc->SetTitle(str1);
		}	
	}
	else
	{
RUN_DIALOG:
		int i=act;
		Setting Host(&i);
		Host.DoModal();
		Connect();
	}
 #endif // CX5
#else  // it's PROXY
	host[act]="Host=100.1.1.10 Prot=telnet Port=80 Term=UnixSpace Lines=40\n";  
	m_channel[act]->Connect(host[act]);	
	Sleep(100);
	return;
#endif // PROXY_DEBUG
	m_channel[act]->Connect(host[act]);	
	SetFocus();
}


BOOL CTerminalView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	// TODO: Add your message handler code here and/or call default
	char *ch;
	if(zDelta>0)
	{
            if((ch=m_channel[act]->Output_Code(12))!=NULL)
				m_channel[act]->Write(ch);
			else
				m_channel[act]->Write("\x1b[A");
	}
	else
	{
			if((ch=m_channel[act]->Output_Code(13))!=NULL)
				m_channel[act]->Write(ch);
			else
				m_channel[act]->Write("\x1b[B");
	}
	return CView::OnMouseWheel(nFlags, zDelta, pt);
}

long GetVSNumber()
{
	char NameBuffer[MAX_PATH];
	char SysNameBuffer[MAX_PATH];
	DWORD VSNumber=0;
	DWORD MCLength;
	DWORD FileSF;

	GetVolumeInformation("C:\\",NameBuffer, sizeof(NameBuffer),&VSNumber,&MCLength,&FileSF,SysNameBuffer,sizeof(SysNameBuffer));
	return(VSNumber);
}

void CTerminalView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();
	// TODO: Add your specialized code here and/or call the base class

	int fd,hi,lo,key,x;
	DWORD k;	
	char buf[256];
	struct stat st;
	if(!stat(path+"bin/UnixSpace.exe",&st))
	{
		if((fd=open(path+"etc/Terminal.key",O_RDONLY))<0)                                         
			goto ASK;
	}
	else
	{
		AfxMessageBox("The file UnixSpace.exe should be located in "+path);
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

void CTerminalView::OnScreen1() 
{
	// TODO: Add your command handler code here
	set_active(0);
}

void CTerminalView::OnScreen2() 
{
	// TODO: Add your command handler code here
	set_active(1);	
}

void CTerminalView::OnScreen3() 
{
	// TODO: Add your command handler code here
	set_active(2);	
}

void CTerminalView::OnScreen4() 
{
	// TODO: Add your command handler code here
	set_active(3);	
}

void CTerminalView::OnHostsetting() 
{
	// TODO: Add your command handler code here
	int i=act;
#ifndef CX5
	Setting Host(&i);

	Host.DoModal();
	if(i>=0)
	{
		int rcon=0;
		if(m_channel[i]==NULL)
			rcon=1;
		set_active(i);
		if(rcon)
			Connect();
	}
#endif
}

void CTerminalView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
}

int CTerminalView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
}

void CTerminalView::OnDestroy() 
{
	CView::OnDestroy();
	
	// TODO: Add your message handler code here	
}
