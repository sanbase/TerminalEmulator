#pragma once


// COpenGL

class COpenGL : public CWnd
{
	DECLARE_DYNAMIC(COpenGL)

public:
	COpenGL();
	virtual ~COpenGL();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);

	void BuildList();
	BOOL SetWindowPixelFormat(HDC hDC);
	BOOL CreateViewGLContext(HDC hDC);
	void RenderScene();
	void Rotate();

	BOOL m_Rotate;

	HGLRC m_hGLContext;
	int m_GLPixelIndex;

	double m_transY;
	double m_transX;
	double m_angle1;
	double m_angle2;

	float m_ScaleX;
	float m_ScaleY;
	float m_ScaleZ;

	GLdouble m_xRotate;
	GLdouble m_yRotate;

	int m_width;
	int m_height;
	int m_exp;

	float m_size;

	CPoint m_RightDownPos;
	CPoint m_LeftDownPos;
	BOOL m_RightButtonDown;
	BOOL m_LeftButtonDown;

	HICON m_hIcon;
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
//	afx_msg void OnNcRButtonDown(UINT nHitTest, CPoint point);
//	afx_msg void OnNcRButtonUp(UINT nHitTest, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
};


