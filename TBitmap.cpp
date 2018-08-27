// TBitmap.cpp: implementation of the TBitmap class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "terminal.h"
#include "TBitmap.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

struct Font_Pool *FontPool;
static int num_fonts=0;

static struct Brush_Pool *BrushPool;
static int num_brush=0;

static struct Pen_Pool *PenPool;
static int num_pen=0;

#define INDIRECT_VIDEO
TBitmap::TBitmap(CWnd* pWnd)
{
	CDC *dc=pWnd->GetDC();

	Canvas = new CDC();
	m_bmp  = new CBitmap();
	CRect rec;
	pWnd->GetClientRect(&rec);
	Width=rec.Width();
	Height=rec.Height();

	Canvas->CreateCompatibleDC(dc);
//	m_bmp->CreateCompatibleBitmap(dc,Width,Height);

	bmi.bmiHeader.biSize		= sizeof bmi.bmiHeader;
	bmi.bmiHeader.biWidth		= Width;
	bmi.bmiHeader.biHeight		= Height;
	bmi.bmiHeader.biPlanes		= 1;
	bmi.bmiHeader.biBitCount	= 32;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biSizeImage	= 0;

	hBitmap = CreateDIBSection(Canvas->m_hDC, &bmi, DIB_RGB_COLORS,(void**)&buffer, 0, 0);
	m_bmp->Attach(hBitmap);

	Canvas->SelectObject(m_bmp);
	Canvas->SetBkMode(TRANSPARENT);
}

TBitmap::TBitmap(CDC *dc,int w, int h)
{

	CBitmap bmp;
	m_bmp=NULL;
	Canvas = new CDC();
	Canvas->CreateCompatibleDC(dc);
	bmp.CreateCompatibleBitmap(dc,w,h);
	Canvas->SelectObject(&bmp);
	Width=w;
	Height=h;
	Canvas->SetBkMode(TRANSPARENT);
}

void clean_var()
{
	for(int i=0;i<num_fonts;i++)
	{
		FontPool[i].name->Empty();
		delete FontPool[i].name;
		delete FontPool[i].font;
	}
	if(FontPool!=NULL)
		free(FontPool);
	for(i=0;i<num_brush;i++)
		delete BrushPool[i].Brush;
	if(BrushPool!=NULL)
		free(BrushPool);
	for(i=0;i<num_pen;i++)
		delete PenPool[i].Pen;
	if(PenPool!=NULL)
		free(PenPool);
}

TBitmap::~TBitmap()
{
	DeleteObject(hBitmap);	
	if(Canvas!=NULL)
		delete Canvas;
	if(m_bmp!=NULL)
		delete m_bmp;
}

CBrush *TBitmap::SetBrush(int color)
{
	for(int i=0;i<num_brush;i++)
	{
		if(BrushPool[i].color==color)
		{
			Canvas->SelectObject(BrushPool[i].Brush);
			return(BrushPool[i].Brush);
		}
	}
	num_brush++;
	BrushPool=(struct Brush_Pool *)realloc(BrushPool,num_brush*sizeof (struct Brush_Pool));
	BrushPool[num_brush-1].color=color;
	BrushPool[num_brush-1].Brush=new CBrush();
	BrushPool[num_brush-1].Brush->CreateSolidBrush(color);
	Canvas->SelectObject(BrushPool[num_brush-1].Brush);
	return BrushPool[num_brush-1].Brush;
}

CPen *TBitmap::SetPen(int color)
{
	for(int i=0;i<num_pen;i++)
	{
		if(PenPool[i].color==color)
		{
			Canvas->SelectObject(PenPool[i].Pen);
			return PenPool[i].Pen;
		}
	}
	++num_pen;
	PenPool=(struct Pen_Pool *)realloc(PenPool,num_pen*sizeof (struct Pen_Pool));
	PenPool[num_pen-1].color=color;
	PenPool[num_pen-1].Pen=new CPen();
	PenPool[num_pen-1].Pen->CreatePen(PS_SOLID,1,color);
	Canvas->SelectObject(PenPool[num_pen-1].Pen);
	return PenPool[num_pen-1].Pen;
}

void TBitmap::SetClientFont(CString Typeface, CString str, int l, int h, bool Bold,bool Italic,int CharSet) 
{
	int Size=h;
	do
	{
		SetClientFont(Typeface,Size,l,h,Bold,Italic,CharSet);
		Size--;
	}
	while(Canvas->GetTextExtent(str).cx>l);
}

void TBitmap::SetClientFont(CString Typeface, int Size, int w, int h, bool Bold,bool Italic,int CharSet) 
{
	for(int i=0;i<num_fonts;i++)
	{
		if(Typeface==*FontPool[i].name && Size==FontPool[i].size && Bold==FontPool[i].bold && Italic==FontPool[i].italic && CharSet==FontPool[i].charset)
		{
			Canvas->SelectObject(FontPool[i].font);
			fn=i;
			return;
		}
	}

 	int Weight = FW_NORMAL;
  	if(Bold)
 		Weight = FW_BOLD; 
 
	int fontHeight=Size+10;
	TEXTMETRIC fm;
/*
	CRect rec;
	CWnd *win=Canvas->GetWindow();
	win->GetClientRect(&rec);
*/
	++num_fonts;
	FontPool=(struct Font_Pool *)realloc(FontPool,num_fonts*sizeof (struct Font_Pool));
	FontPool[num_fonts-1].font=NULL;
	FontPool[num_fonts-1].name=new CString(Typeface);
	FontPool[num_fonts-1].size=Size;
	FontPool[num_fonts-1].bold=Bold;
	FontPool[num_fonts-1].italic=Italic;
	FontPool[num_fonts-1].charset=CharSet;
#ifdef CX5
BEGIN:
#endif
  	do
	{
		if(FontPool[num_fonts-1].font!=NULL)
			delete FontPool[num_fonts-1].font;
		FontPool[num_fonts-1].font=new CFont();
 		FontPool[num_fonts-1].font->CreateFont(fontHeight, 0, 0, 0, Weight, Italic, 0, 0, 
                                CharSet, OUT_DEFAULT_PRECIS, 
                                CLIP_DEFAULT_PRECIS,DRAFT_QUALITY   ,
                                DEFAULT_PITCH | FF_DONTCARE, Typeface);
		Canvas->SelectObject(FontPool[num_fonts-1].font);
		fn=num_fonts-1;
		Canvas->GetOutputTextMetrics(&fm);
		fontHeight--;
  	}
	while(fm.tmHeight+1>Size);

	Canvas->GetCharWidth(0,255,FontPool[fn].CharWidth);
#ifdef CX5
	if(w>0 && FontPool[fn].CharWidth['@']*80>w)
	{
		fontHeight--;
		goto BEGIN;
	}
#endif
	return;
}

struct Font_Pool *TBitmap::Font()
{
	return(FontPool+fn);
}

void TBitmap::FillRect(CRect *rec,int color)
{
	CBrush *br=SetBrush(color);
	Canvas->FillRect(rec,br);
}

void TBitmap::Elipse(CRect *rec,int color)
{
	CBrush *br=SetBrush(color);
//	Canvas->Ellipse(rec);
	Canvas->DrawEdge(rec,EDGE_SUNKEN,BF_RECT);
}

void TBitmap::Resize(int w, int h)
{
/*
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(Canvas,w,h);
	Canvas->SelectObject(&bmp);
	Width=w;
	Height=h;
*/
}

//*********************