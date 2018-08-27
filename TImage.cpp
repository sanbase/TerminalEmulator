// TImage.cpp: implementation of the TImage class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TImage.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//******************************************************************************
TImage::TImage():m_DC(NULL),m_Bitmap(NULL),m_Width(0),m_Height(0)
{
}

//*****************************************************************************
TImage::TImage(HDC dc, int width, int height)
{
	m_Bitmap = CreateCompatibleBitmap(dc, width, height);
	m_DC = CreateCompatibleDC(dc);
	m_Obj = SelectObject(m_DC, m_Bitmap);
	m_Width = width;
	m_Height = height;
}

//******************************************************************************
TImage::~TImage()
{
	Clear();
}

//******************************************************************************
void TImage::Clear()
{
	if (m_DC)
	{
		SelectObject(m_DC, m_Obj);
		DeleteDC(m_DC);
		m_DC = NULL;
	}
	if (m_Bitmap) 
		DeleteObject(m_Bitmap);
	m_Bitmap = NULL;
	m_Width = 0;
	m_Height = 0;
}

//******************************************************************************
HDC	TImage::GetDC()
{
	return m_DC;
}

//******************************************************************************
void TImage::CheckDC(HDC dc)
{
	if (!m_DC)
	{
		m_DC = CreateCompatibleDC(dc);
		if (m_Bitmap)
			m_Obj = SelectObject(m_DC, m_Bitmap);
	}
}

//******************************************************************************
HBITMAP	TImage::GetHandle()
{
	return m_Bitmap;
}

//******************************************************************************
bool TImage::LoadFromResource(HINSTANCE instance, LPCTSTR bitmapName)
{
	Clear();
	m_Bitmap = LoadBitmap(instance, bitmapName);
	GetSize();
	return m_Bitmap == NULL;
}

//******************************************************************************
bool TImage::LoadFromFile(LPCTSTR bitmapName)
{
	Clear();
	m_Bitmap = (HBITMAP)LoadImage(NULL, bitmapName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	GetSize();
	return m_Bitmap == NULL;
}

//******************************************************************************
void TImage::Draw(long x, long y, long width, long height, HDC dc, long op, long srcx, long srcy)
{
	if (!m_Bitmap) 
		return;
	if (!m_DC)
	{
		m_DC = CreateCompatibleDC(dc);
		m_Obj = SelectObject(m_DC, m_Bitmap);
	}
	BitBlt(dc, x, y, width, height, m_DC, srcx, srcy, op);
}



//******************************************************************************
void TImage::Resize(int width, int height)
{
	if (m_Bitmap)
	{
		// create new
		if (m_DC)
			SelectObject(m_DC, m_Obj);
		HBITMAP tBMP = (HBITMAP)CopyImage(m_Bitmap, IMAGE_BITMAP, width, height, LR_COPYDELETEORG);
		if (m_DC)
			m_Obj = SelectObject(m_DC, tBMP);
		m_Bitmap = tBMP;
	}
	else if (m_DC)
	{
		m_Bitmap = CreateCompatibleBitmap(m_DC, width, height);
		m_Obj = SelectObject(m_DC, m_Bitmap);
	}
	else
		return;
	m_Width = width;
	m_Height = height;
}

//******************************************************************************
void TImage::GetSize()
{
	if (m_Bitmap == NULL)
		return;
	BITMAP bm;
	GetObject(m_Bitmap, sizeof( bm ), &bm );
	m_Width = bm.bmWidth;
	m_Height = bm.bmHeight;
}

//******************************************************************************
int	TImage::GetWidth()
{ 
	return m_Width;
}

//******************************************************************************
int TImage::GetHeight()
{
	return m_Height;
}
