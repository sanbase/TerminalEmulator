// screen.h: interface for the screen class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCREEN_H__BD7D57D4_9D8F_4F14_A5F9_860EC2B252E1__INCLUDED_)
#define AFX_SCREEN_H__BD7D57D4_9D8F_4F14_A5F9_860EC2B252E1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define ESC  27
#define TSTATE_DATA             0
#define TSTATE_ESC              1       // ESC
#define TSTATE_CSI              2       // ESC [
#define TSTATE_ESCSQUARE        3       // ESC #
#define TSTATE_START            4
#define TSTATE_ESCO             5       // ESC ? 
#define TSTATE_ANSI				6		// ESC 2 - ansi color

class Terminal;

//#include "string.h"

struct clr   { unsigned char bg, fg, font, frame; unsigned char atr1:4,atr2:4,bwlt:1,x1:7;};
struct pics  { unsigned char ch; struct clr clr;};

class COLOR
{
public:
	int r,g,b;
	COLOR()
	{
		r=g=b=0;
	}
	COLOR(int i)
	{
		b=i&0xff;
		g=(i>>8)&0xff;
		r=(i>>16)&0xff;
	}
	COLOR(int r, int g, int b)
	{
		this->r=r;
		this->g=g;
		this->b=b;
	}
	int   getRed()
	{
		return r;
	}
	int   getGreen()
	{
		return g;
	}
	int   getBlue()
	{
		return b;
	}
	COLORREF value()
	{
		return(r+(g<<8)+(b<<16));
	}
	bool   equals(COLOR color)
	{
		return(color.getRed()==r && color.getBlue()==b && color.getGreen()==g);
	}
};

class screen
{
private:
	struct pics *client;
	int x,y;
	int mX,mY;
	int cW,cH;
	struct clr S_color;
	int x_c,y_c;
	Terminal *term;

// X - COLUMNS, Y - ROWS
	int  R,C;
	int  Sc,Sr,Sa;
	int  insertmode;
    int lastwaslf;
	boolean unirus;

	int term_state;
	int Tabs[256];
    char buf[256];
    int ch_count;

	int  DCEvars[10];
	CString Name_Image;
	int  DCEvar;
	int coord_x0,coord_y0,coord_x1,coord_y1,count;
	void   clean_vars();
    int light;
public:
	screen(int x, int y, Terminal *io);
	virtual ~screen();
	int		__fastcall Width(int x=0);
	int		Height(int y=0);
	int		charWidth(int w=0);
	int		charHeight(int h=0);
	void	Set_Color(struct clr color);
	void	Set_Font(unsigned char font);
	void	Color(int x, int y, struct clr color);
	void	Set_Char(int x, int y, char ch);
	CString  Chars(int y, int x, int l);
	struct	clr  __fastcall Attr(int y,int x);
	unsigned char	Value(int y, int x);
	struct	pics	__fastcall Char(int x,int y);
	struct	clr  Color(int x,int y);
	struct	clr  currColor();
	int		currX();
	int		currY();
	void	_SetCursor(int row,int col);
	void	erase_screen();
	void	erase_screen(struct clr color);
	void	erase_line();
	void	erase_line(struct clr color);
	void	erase_area(int x,int y,int l,int h);
	void	Erase_Area(int x,int y,int l,int h);
	void	erase_area(int x,int y,int l,int h,struct clr color);
	void	set_bg_box(int x,int y,int l,int h,int color);
	void	black_white(int x,int y,int l,int h,int atr);
	void	putChar(unsigned char ch);
	void	putChar(int x, int y, char ch);
	void	putChar(int x, int y, char ch, struct clr color);
	void	putString(CString);
	void	pos(int x, int y);
	void	putString(char *str,int len=0);
	void	insertChar(int x, int y, char ch);
	void	insertChar(int x, int y, char ch, struct clr color);
	void	deleteChar(int x, int y);
	void	insertLine(int y);
	void	insertLine(int l, int n);
	void	insertLine(int x, int y, int w, int h, int l);
	void	deleteLine(int x, int y, int w, int h, int l);
	void	deleteLine(int y);
	void	deleteLine(int y, int n);
	void	deleteArea(int x, int y, int w, int h);

	void	put_frame(int x,int y,int frame);
	void	put_font(int x,int y,int font);
	void	put_a1(int x,int y,int atr1);
	void	put_a2(int x,int y,int atr2);

	void	copy(screen *tmp);
	void	setCursorPos();
    void	Set_Type(CString terminal);
};


#endif // !defined(AFX_SCREEN_H__BD7D57D4_9D8F_4F14_A5F9_860EC2B252E1__INCLUDED_)
