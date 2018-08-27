// terminalView.h : interface of the CTerminalView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_TERMINALVIEW_H__839C0ED7_4E3F_497F_9E85_87F0E50E17C2__INCLUDED_)
#define AFX_TERMINALVIEW_H__839C0ED7_4E3F_497F_9E85_87F0E50E17C2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class TBitmap;
class CTerminalDoc;
class channel;
class COpenGL;

struct VE
{
	int eff;
	int n;
	int x;
	int y;
	int l;
	int h;
};

class CTerminalView : public CView
{
	friend class terminalIO;
	friend class Terminal;
	friend class CConteXtView;
	friend class channel;
protected: // create from serialization only
	CTerminalView();
	DECLARE_DYNCREATE(CTerminalView)

// Attributes
public:
	CTerminalDoc* GetDocument();
// Operations
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTerminalView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTerminalView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	HDC m_hDC;

// Generated message map functions
protected:
	//{{AFX_MSG(CTerminalView)
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnConnect();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnScreen1();
	afx_msg void OnScreen2();
	afx_msg void OnScreen3();
	afx_msg void OnScreen4();
	afx_msg void OnHostsetting();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	void set_active(int i);
	char pressedKey;
	HANDLE Terminal_PID[4];
    DWORD m_nHotKeyID;

	unsigned long		m_Thread;			// thread handle
	bool volatile		m_StopThread;		// stop flag
	bool volatile		m_Active;			// perform animation 
	RGBQUAD *stored;
	struct VE visual_eff;
	int scale;
	int isPaint;
	HGLRC m_hGLContext;
	int m_GLPixelIndex;
	COpenGL *m_gl;
	void VisualEff(CDC *pDC);
public:
	CString host[4];
	channel **m_channel;
	CRITICAL_SECTION	m_DrawSynchro;
	int act;
	int num_channels;
    bool reconnect;
	void paint();
	void init();
	void Run();
	void mPaint();
	void Visual_Eff_Exec(int,int,int,int,int,int);
	virtual void Connect();
	void cancel();
//	void RunBrowser();
//	afx_msg void OnPaint();
};

#ifndef _DEBUG  // debug version in terminalView.cpp
inline CTerminalDoc* CTerminalView::GetDocument()
   { return (CTerminalDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TERMINALVIEW_H__839C0ED7_4E3F_497F_9E85_87F0E50E17C2__INCLUDED_)
