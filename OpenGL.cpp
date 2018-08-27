// OpenGL.cpp : implementation file
//

#include "stdafx.h"
#include "terminal.h"
#include "OpenGL.h"
#include "opengl.h"

#include <gl/gl.h>
#include <gl/glu.h>
#include "opengl.h"

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "glaux.lib")			
// COpenGL

IMPLEMENT_DYNAMIC(COpenGL, CWnd)
COpenGL::COpenGL()
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_hGLContext = NULL;
	m_GLPixelIndex = 0;


	m_xRotate = 0;
	m_yRotate = 0;
	m_Rotate = FALSE;

	m_ScaleX = 1.0f;
	m_ScaleY = 1.0f;
	m_ScaleZ = 1.0f;

	m_size = 1.5;
	m_exp = 9;

	m_RightButtonDown = FALSE;
	m_LeftButtonDown = FALSE;
}

COpenGL::~COpenGL()
{
}


BEGIN_MESSAGE_MAP(COpenGL, CWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
//	ON_WM_NCRBUTTONDOWN()
//	ON_WM_NCRBUTTONUP()
ON_WM_RBUTTONDOWN()
ON_WM_RBUTTONUP()
END_MESSAGE_MAP()



// COpenGL message handlers


//****************************************************************************************************************************
int MySetPixelFormat(HDC hdc)
{
	PIXELFORMATDESCRIPTOR pfd = { 
	    sizeof(PIXELFORMATDESCRIPTOR),    // size of this pfd 
	    1,                                // version number 
	    PFD_DRAW_TO_WINDOW |              // support window 
	    PFD_SUPPORT_OPENGL |              // support OpenGL 
	    PFD_DOUBLEBUFFER,                 // double buffered 
	    PFD_TYPE_RGBA,                    // RGBA type 
	    24,                               // 24-bit color depth 
	    0, 0, 0, 0, 0, 0,                 // color bits ignored 
	    0,                                // no alpha buffer 
	    0,                                // shift bit ignored 
	    0,                                // no accumulation buffer 
	    0, 0, 0, 0,                       // accum bits ignored 
	    32,                               // 32-bit z-buffer     
	    0,                                // no stencil buffer 
	    0,                                // no auxiliary buffer 
	    PFD_MAIN_PLANE,                   // main layer 
	    0,                                // reserved 
	    0, 0, 0                           // layer masks ignored 
	}; 
	
	int  iPixelFormat; 
 
	// get the device context's best, available pixel format match 
	if((iPixelFormat = ChoosePixelFormat(hdc, &pfd)) == 0)
	{
		MessageBox(NULL, "ChoosePixelFormat Failed", NULL, MB_OK);
		return 0;
	}
	 
	// make that match the device context's current pixel format 
	if(SetPixelFormat(hdc, iPixelFormat, &pfd) == FALSE)
	{
		MessageBox(NULL, "SetPixelFormat Failed", NULL, MB_OK);
		return 0;
	}

	return 1;
}

int COpenGL::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
//	MySetPixelFormat(::GetDC(m_hWnd));

	HWND hWnd = GetSafeHwnd();
	HDC hDC = ::GetDC(hWnd);

	if(SetWindowPixelFormat(hDC)==FALSE)
		return 0;
	
	if(CreateViewGLContext(hDC)==FALSE)
		return 0;

	glClearColor(0.5f,0.8f,0.7f,1.0);

	glPolygonMode(GL_FRONT,GL_FILL);
	glPolygonMode(GL_BACK,GL_FILL);

	// Gouraud
	glShadeModel(GL_SMOOTH);

	BuildList();

//	SetTimer(0,10,NULL);
	return 0;
}
BOOL COpenGL::SetWindowPixelFormat(HDC hDC)
{
	PIXELFORMATDESCRIPTOR pixelDesc;

	pixelDesc.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pixelDesc.nVersion = 1;

	pixelDesc.dwFlags = PFD_DRAW_TO_WINDOW | 
					PFD_SUPPORT_OPENGL |
					PFD_DOUBLEBUFFER |
					PFD_STEREO_DONTCARE;

	pixelDesc.iPixelType = PFD_TYPE_RGBA;
	pixelDesc.cColorBits = 32;
	pixelDesc.cRedBits = 8;
	pixelDesc.cRedShift = 16;
	pixelDesc.cGreenBits = 8;
	pixelDesc.cGreenShift = 8;
	pixelDesc.cBlueBits = 8;
	pixelDesc.cBlueShift = 0;
	pixelDesc.cAlphaBits = 0;
	pixelDesc.cAlphaShift = 0;
	pixelDesc.cAccumBits = 64;
	pixelDesc.cAccumRedBits = 16;
	pixelDesc.cAccumGreenBits = 16;
	pixelDesc.cAccumBlueBits = 16;
	pixelDesc.cAccumAlphaBits = 0;
	pixelDesc.cDepthBits = 32;
	pixelDesc.cStencilBits = 8;
	pixelDesc.cAuxBuffers = 0;
	pixelDesc.iLayerType = PFD_MAIN_PLANE;
	pixelDesc.bReserved = 0;
	pixelDesc.dwLayerMask = 0;
	pixelDesc.dwVisibleMask = 0;
	pixelDesc.dwDamageMask = 0;

	m_GLPixelIndex = ChoosePixelFormat(hDC,&pixelDesc);
	if(m_GLPixelIndex==0) // Choose default
	{
		m_GLPixelIndex = 1;
		if(DescribePixelFormat(hDC,m_GLPixelIndex,sizeof(PIXELFORMATDESCRIPTOR),&pixelDesc)==0)
		{
			return FALSE;
		}
	}

	if(SetPixelFormat(hDC,m_GLPixelIndex,&pixelDesc)==FALSE)
	{
		return FALSE;
	}
	return TRUE;
}

BOOL COpenGL::CreateViewGLContext(HDC hDC)
{
	m_hGLContext = wglCreateContext(hDC);

	if(m_hGLContext==NULL)
		return FALSE;

	if(wglMakeCurrent(hDC,m_hGLContext)==FALSE)
		return FALSE;

	return TRUE;
}
void COpenGL::OnDestroy()
{
	CWnd::OnDestroy();

	// TODO: Add your message handler code here
	if(wglGetCurrentContext() != NULL)
		wglMakeCurrent(NULL,NULL);

	if(m_hGLContext != NULL)
	{
		wglDeleteContext(m_hGLContext);
		m_hGLContext = NULL;
	}
}

void COpenGL::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	RenderScene();
	SwapBuffers(dc.m_ps.hdc);
//	CWnd::OnPaint();
}

void COpenGL::RenderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_LIGHTING);

	// Set up for scene
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// position the eye relative to scene
	gluLookAt(	0.0f,0.0f,8.0f,	// eye
				0.0f,0.0f,0.0f,	// looking at
				0.0f,1.0f,0.0f);// is up

	// use a quadric to draw a circle
	GLUquadricObj *glpQ;
	glpQ=gluNewQuadric();
	// setMaterial kind of yellow
    GLfloat mat_ambient1[] = { 0.4f, 0.8f, 0.7f, 1.0f };
    GLfloat mat_diffuse1[] = { 0.7f, 0.2f, 0.6f, 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient1);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse1);
	gluSphere(glpQ,1.0,30,30);

	// setMaterial kind of yellow
    GLfloat mat_ambient2[] = { 0.7f, 0.8f, 0.4f, 1.0f };
    GLfloat mat_diffuse2[] = { 0.6f, 0.5f, 0.2f, 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient2);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse2);
	glRotated(m_xRotate/1.3, 1.0, 0.0, 0.0);
	glRotated(m_yRotate/1.5, 0.0, 1.0, 0.0);
	gluCylinder(glpQ,1.3,1.3,2.0,20,20);
	gluDeleteQuadric(glpQ);
	glFlush();

	glDisable(GL_LIGHTING);
	glPushMatrix();

	// Position / translation
	glTranslated(1.3,0.0,-1.5);
	glRotated(m_xRotate, 1.0, 0.0, 0.0);
	glRotated(m_yRotate, 0.0, 1.0, 0.0);
	glScalef(m_ScaleX,m_ScaleY,m_ScaleZ);
  
	::glCallList(1);
	glPopMatrix();

	// finnish drawing
	glFlush();
}
void COpenGL::BuildList()
{
	::glNewList(1,GL_COMPILE_AND_EXECUTE);

//	glColor3f(0.6f,0.7f,0.3f);
	glShadeModel(GL_SMOOTH);
	
	float x = m_size;

	glBegin(GL_POLYGON);
		glNormal3d(0.0,0.0,-1.0);
		glColor3ub(255,0,0);	glVertex3d( x,  x, x);
		glColor3ub(0,255,0);	glVertex3d( x, -x, x);
		glColor3ub(0,0,255);	glVertex3d(-x, -x, x);
		glColor3ub(255,255,0);	glVertex3d(-x,  x, x);
	glEnd();

	glBegin(GL_POLYGON);
		glNormal3d(0.0,0.0,1.0);
		glColor3ub(255,0,0);	glVertex3d(-x,  x, -x);
		glColor3ub(0,255,0);	glVertex3d(-x, -x, -x);
		glColor3ub(0,0,255);	glVertex3d( x, -x, -x);
		glColor3ub(255,255,0);	glVertex3d( x,  x, -x);
	glEnd();

	glBegin(GL_POLYGON);
		glNormal3d(-1.0,0.0,0.0);
		glColor3ub(255,0,0);	glVertex3d( x,  x,  x);
		glColor3ub(0,255,0);	glVertex3d( x,  x, -x);
		glColor3ub(0,0,255);	glVertex3d( x, -x, -x);
		glColor3ub(255,255,0);	glVertex3d( x, -x,  x);
	glEnd();

	glBegin(GL_POLYGON);
		glNormal3d(-1.0,0.0,0.0);
		glColor3ub(255,0,0);	glVertex3d(-x,  x,  x);
		glColor3ub(0,255,0);	glVertex3d(-x,  x, -x);
		glColor3ub(0,0,255);	glVertex3d(-x, -x, -x);
		glColor3ub(255,255,0);	glVertex3d(-x, -x,  x);
	glEnd();

	glBegin(GL_POLYGON);
		glNormal3d(0.0,1.0,0.0);
		glColor3ub(255,0,0);	glVertex3d(-x, -x,  x);
		glColor3ub(0,255,0);	glVertex3d( x, -x,  x);
		glColor3ub(0,0,255);	glVertex3d( x, -x, -x);
		glColor3ub(255,255,0);	glVertex3d(-x, -x, -x);
	glEnd();


	glBegin(GL_POLYGON);
		glNormal3d(0.0,1.0,0.0);
		glColor3ub(255,0,0);	glVertex3d(-x,  x,  x);
		glColor3ub(0,255,0);	glVertex3d( x,  x,  x);
		glColor3ub(0,0,255);	glVertex3d( x,  x, -x);
		glColor3ub(255,255,0);	glVertex3d(-x,  x, -x);
	glEnd();

	::glEndList();
}

void COpenGL::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	GLsizei width,height;
	GLdouble aspect;

	width = cx;
	height = cy;

	if(cy==0)
		aspect = (GLdouble)width;
	else
		aspect = (GLdouble)width/(GLdouble)height;

	glViewport(0,0,width,height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45,aspect,0.01,100.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDrawBuffer(GL_BACK);
	glEnable(GL_DEPTH_TEST);
/*
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
*/
	// Lights, material properties
	GLfloat	ambientProperties[]  = {0.6f, 0.6f, 0.7f, 1.0f};
	GLfloat	diffuseProperties[]  = {0.8f, 0.8f, 0.8f, 1.0f};
	GLfloat	specularProperties[] = {1.0f, 1.0f, 1.0f, 1.0f};
	
	glLightfv( GL_LIGHT0, GL_AMBIENT, ambientProperties);
	glLightfv( GL_LIGHT0, GL_DIFFUSE, diffuseProperties);
	glLightfv( GL_LIGHT0, GL_SPECULAR, specularProperties);

	glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, 1.0);
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

	// Default : lighting
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);

	glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,diffuseProperties);

//    auxInitPosition( 50, 10, 400, 400);
//    auxInitDisplayMode( AUX_RGB | AUX_DEPTH | AUX_DOUBLE );
}

void COpenGL::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if(m_LeftButtonDown)
	{
		CSize rotate = m_LeftDownPos - point;
		m_LeftDownPos = point;
		m_yRotate -= rotate.cx;
		m_xRotate -= rotate.cy;
		InvalidateRect(NULL,FALSE);
	}
	if(m_RightButtonDown)
	{
		CSize rotate = m_RightDownPos - point;
		m_RightDownPos = point;
		m_ScaleX -= (float)((rotate.cx)/100.0);
		m_ScaleY -= (float)((rotate.cy)/100.0);
		InvalidateRect(NULL,FALSE);
	}
	CWnd::OnMouseMove(nFlags, point);
}

void COpenGL::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	m_LeftButtonDown = TRUE;
	m_LeftDownPos = point;
	CWnd::OnLButtonDown(nFlags, point);
}

void COpenGL::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	m_LeftButtonDown = FALSE;
	CWnd::OnLButtonUp(nFlags, point);
}


void COpenGL::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	m_RightButtonDown = TRUE;
	m_RightDownPos = point;
	CWnd::OnRButtonDown(nFlags, point);
}

void COpenGL::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	m_RightButtonDown = FALSE;
	CWnd::OnRButtonUp(nFlags, point);
}
