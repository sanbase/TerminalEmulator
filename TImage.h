// TImage.h: interface for the TImage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TIMAGE_H__789DCCA2_26A9_480F_884D_5BB69FB81BC9__INCLUDED_)
#define AFX_TIMAGE_H__789DCCA2_26A9_480F_884D_5BB69FB81BC9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class TImage
{
 public:
				// constructor
				TImage();
				// constructor
				TImage(HDC dc, int width, int height);
				// destructor
	 virtual	~TImage();
				// return associated DC
	 HDC		GetDC();
				// create internal DC
	 void		CheckDC(HDC dc);
				// return bitmap handle
	 HBITMAP	GetHandle();
				// clear object
	 void		Clear();
				// load image from resource
	 bool		LoadFromResource(HINSTANCE instance, LPCTSTR bitmapName);
				// load image from resource
	 bool		LoadFromFile(LPCTSTR bitmapName);
				// draw 
	 void		Draw(long x, long y, long width, long height, HDC dc, long Op, long srcx = 0, long srcy = 0);
				// resize bitmap
	 void		Resize(int width, int height);
				// return width
	 int		GetWidth();
				// return width
	 int		GetHeight();
	 HBITMAP	m_Bitmap;
 private:
	 void		GetSize();

	 HGDIOBJ	m_Obj;
	 HDC		m_DC;		
	 int		m_Width;
	 int		m_Height;
	 struct tagLOGBRUSH Style;
};


#endif // !defined(AFX_TIMAGE_H__789DCCA2_26A9_480F_884D_5BB69FB81BC9__INCLUDED_)
