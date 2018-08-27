// TBitmap.h: interface for the TBitmap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TBITMAP_H__630CED60_8A8B_4980_A678_76C319D7DFCB__INCLUDED_)
#define AFX_TBITMAP_H__630CED60_8A8B_4980_A678_76C319D7DFCB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

struct Font_Pool
{
	CString *name;
	int size;
	bool bold;
	bool italic;
	CFont *font;
	int CharWidth[256];
	int charset;
};

struct Brush_Pool
{
	int color;
	CBrush *Brush;
};

struct Pen_Pool
{
	int color;
	CPen *Pen;
};

class TBitmap  
{
public:
	TBitmap(CWnd* pWnd);
	TBitmap(CDC *dc,int w, int h);
	virtual ~TBitmap();
	void SetClientFont(CString Typeface, int Size, int w, int h, bool Bold=false,bool Italic=false,int charser=RUSSIAN_CHARSET);
	void SetClientFont(CString Typeface, CString str, int l, int h, bool Bold,bool Italic,int CharSet);
	CBrush *SetBrush(int color);
	CPen *SetPen(int color);

	void FillRect(CRect *rec,int color);
	void Elipse(CRect *rec,int color);
	void Resize(int w, int h);

	CDC *Canvas;
	CBitmap *m_bmp;

	BITMAPINFO bmi;
	RGBQUAD *buffer;
	HBITMAP hBitmap;

	int Width;
	int Height;
	int fn;
	struct Font_Pool *Font();
};

#endif // !defined(AFX_TBITMAP_H__630CED60_8A8B_4980_A678_76C319D7DFCB__INCLUDED_)
