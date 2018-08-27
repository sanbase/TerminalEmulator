// FrameGL.cpp : implementation file
//

#include "stdafx.h"
#include "terminal.h"
#include "FrameGL.h"
#include "framegl.h"
#include "terminalView.h"
#include "channel.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

///////////////////////////////// SET UP PIXEL FORMAT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function sets the pixel format for OpenGL.
/////
///////////////////////////////// SET UP PIXEL FORMAT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "glaux.lib")					

/////////////////////////////////////////////////////////////////////////////
// FrameGL

IMPLEMENT_DYNCREATE(FrameGL, CFrameWnd)

FrameGL::FrameGL()
{
	m_GLPixelIndex = 0;
}

FrameGL::~FrameGL()
{
}


BEGIN_MESSAGE_MAP(FrameGL, CFrameWnd)
	//{{AFX_MSG_MAP(FrameGL)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_SIZE()
	ON_WM_CHAR()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_WM_KEYDOWN()
	ON_WM_MOVE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// FrameGL message handlers

BOOL FrameGL::PreCreateWindow(CREATESTRUCT& cs) 
{
	// TODO: Add your specialized code here and/or call the base class
//	cs.style |= (WS_CLIPCHILDREN | WS_CLIPSIBLINGS); 
	return CFrameWnd::PreCreateWindow(cs);
}

void FrameGL::OnSize(UINT nType, int cx, int cy) 
{
	CFrameWnd::OnSize(nType, cx, cy);
	
	HWND hWnd = GetSafeHwnd();
	HDC hDC = ::GetDC(hWnd);
	//TRACE("Activate view, set active OpenGL rendering context...\n");
	wglMakeCurrent(hDC,hGLRC);
	// Set OpenGL perspective, viewport and mode
	double aspect = (cy == 0) ? cx : (double)cx/(double)cy;
	
	glViewport(0,0,cx,cy);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45,aspect,0.1,1000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDrawBuffer(GL_BACK);

	glLineWidth(0.05);
	glEnable(GL_LINE_SMOOTH);

	glFrontFace(GL_CW);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	GLfloat ambientLight[] = { 0.3f, 0.3f,0.3f, 1.0f };
	GLfloat diffuseLight[] = { 0.7f, 0.7f,0.7f, 1.0f };
	GLfloat lightPos[] = {-50.0f, 50.0f,100.0f, 1.0f };

	glEnable(GL_LIGHTING);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	glEnable(GL_LIGHT0);

	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK,GL_DIFFUSE);

	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);
    
	glEnable(GL_NORMALIZE);


	// Release
	::ReleaseDC(hWnd,hDC);
}

int FrameGL::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	HWND hWnd = GetSafeHwnd();
	HDC hDC = ::GetDC(hWnd);

	if (SetWindowPixelFormat(hDC)==FALSE)
		return 0;
	
	if (CreateViewGLContext(hDC)==FALSE)
		return 0;

	return 0;
}

void FrameGL::OnDestroy() 
{
	if(wglGetCurrentContext()!=NULL) 
		wglMakeCurrent(NULL, NULL) ;
   
	if(hGLRC!=NULL)
	{
		wglDeleteContext(hGLRC);
		hGLRC = NULL;
	}
//	delete pdc;
	CFrameWnd::OnDestroy();
}

//********************************************
// CreateViewGLContext
// Create an OpenGL rendering context
//********************************************
BOOL FrameGL::CreateViewGLContext(HDC hDC)
{
	hGLRC = wglCreateContext(hDC);
	
	if(hGLRC==NULL)
		return FALSE;
	
	if(wglMakeCurrent(hDC,hGLRC)==FALSE)
		return FALSE;
	
	return TRUE;
}

int FrameGL::SetWindowPixelFormat(HDC hDC)
{
	PIXELFORMATDESCRIPTOR pixelDesc;

	pixelDesc.nSize		= sizeof(PIXELFORMATDESCRIPTOR);
	pixelDesc.nVersion	= 1;

	pixelDesc.dwFlags	=PFD_DRAW_TO_WINDOW | 
						 PFD_SUPPORT_OPENGL |
	     				 PFD_STEREO_DONTCARE|
		    			 PFD_DOUBLEBUFFER;     

	pixelDesc.iPixelType		= PFD_TYPE_RGBA;
	pixelDesc.cColorBits		= 32;
	pixelDesc.cRedBits			= 8;
	pixelDesc.cRedShift			= 16;
	pixelDesc.cGreenBits		= 8;
	pixelDesc.cGreenShift		= 8;
	pixelDesc.cBlueBits			= 8;
	pixelDesc.cBlueShift		= 0;
	pixelDesc.cAlphaBits		= 0;
	pixelDesc.cAlphaShift		= 0;
	pixelDesc.cAccumBits		= 64;	
	pixelDesc.cAccumRedBits		= 16;
	pixelDesc.cAccumGreenBits	= 16;
	pixelDesc.cAccumBlueBits	= 16;
	pixelDesc.cAccumAlphaBits	= 0;
	pixelDesc.cDepthBits		= 32;
	pixelDesc.cStencilBits		= 8;
	pixelDesc.cAuxBuffers		= 0;
	pixelDesc.iLayerType		= PFD_MAIN_PLANE;
	pixelDesc.bReserved			= 0;
	pixelDesc.dwLayerMask		= 0;
	pixelDesc.dwVisibleMask		= 0;
	pixelDesc.dwDamageMask		= 0;

	m_GLPixelIndex = ChoosePixelFormat( hDC, &pixelDesc);
	if (m_GLPixelIndex==0) // Let's choose a default index.
	{
		m_GLPixelIndex = 1;	
		if (DescribePixelFormat(hDC, m_GLPixelIndex, sizeof(PIXELFORMATDESCRIPTOR), &pixelDesc)==0)
		{
			return FALSE;
		}
	}

	if (SetPixelFormat( hDC, m_GLPixelIndex, &pixelDesc)==FALSE)
	{
		return FALSE;
	}

	return TRUE;
}

void FrameGL::OnPaint() 
{
	display();
}

void FrameGL::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CFrameWnd::OnMouseMove(nFlags, point);
}

void FrameGL::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CFrameWnd::OnLButtonDown(nFlags, point);
}

void FrameGL::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default

	CFrameWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

void FrameGL::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	
	CFrameWnd::OnChar(nChar, nRepCnt, nFlags);
}

void FrameGL::display()
{
	CPaintDC dc(this); 

	// Useful in multidoc templates
	HWND hWnd = GetSafeHwnd();
	HDC hDC = ::GetDC(hWnd);
	wglMakeCurrent(hDC,hGLRC);
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glPushMatrix();
/*	
	// Position / translation / scale
	glTranslated(m_xTranslation,m_yTranslation,m_zTranslation);
	glRotatef(m_xRotation, 1.0, 0.0, 0.0);
	glRotatef(m_yRotation, 0.0, 1.0, 0.0);
	glRotatef(m_zRotation, 0.0, 0.0, 1.0);
	glScalef(m_xScaling,m_yScaling,m_zScaling);
*/

 static int time = 0;


      glPushMatrix();  
      glRotated(time/2, 0.0, 1.0, 0.0);      
        
       glLineWidth(5.0f);
       glColor3f(1.0f,0.0f,0.0f);

       glBegin(GL_LINES);
        glVertex3d(-0.3,0.0,0.0);
        glVertex3d(1.5,0.0,0.0);
       glEnd();


       glPushMatrix();
         glRotated(2*time, 1,0,0);
         glTranslated(-0.3,0,0);
         glColor3f(0,0,1);
       
         glPushMatrix();
           glRotated(90,0,1,0);
	   glLineWidth(1);
           auxWireTorus(0.2, 0.7);
	  glPopMatrix();  
       
		 
         glLineWidth(7);
         glColor3f(0,1,0); 

        glBegin(GL_LINES);
           glVertex3d(0,0,0);
           glVertex3d(0,1,0);
           glVertex3d(0,0,0);
           glVertex3d(0,-0.5,1);
           glVertex3d(0,0,0);
           glVertex3d(0,-0.5,-1);
        glEnd();
       glPopMatrix(); 
  
       glPopMatrix();
   
       time++;


	glPopMatrix();

	// Double buffer
	SwapBuffers(dc.m_ps.hdc);
	glFlush();

	// Release
	::ReleaseDC(hWnd,hDC);

	Sleep(40);

}

void FrameGL::OnMove(int x, int y)
{
	CFrameWnd::OnMove(x, y);
	// TODO: Add your message handler code here
	main_frame->m_channel[main_frame->act]->update[0]=true;
}
