#if !defined(AFX_FRAMEGL_H__C0B5A965_EE1B_4993_B2B0_957C7E323D69__INCLUDED_)
#define AFX_FRAMEGL_H__C0B5A965_EE1B_4993_B2B0_957C7E323D69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FrameGL.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// FrameGL frame

class FrameGL : public CFrameWnd
{
	friend class CTerminalView;
	DECLARE_DYNCREATE(FrameGL)
private:
	CClientDC *pdc;
	HGLRC hGLRC;
	int SetWindowPixelFormat(HDC);
	CTerminalView *main_frame;
	BOOL CreateViewGLContext(HDC hDC);
	int m_GLPixelIndex;
protected:
	FrameGL();           // protected constructor used by dynamic creation

// Attributes
public:
	void display();
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(FrameGL)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~FrameGL();

	// Generated message map functions
	//{{AFX_MSG(FrameGL)
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnMove(int x, int y);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FRAMEGL_H__C0B5A965_EE1B_4993_B2B0_957C7E323D69__INCLUDED_)
