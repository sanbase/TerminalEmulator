// Terminal.h: interface for the Terminal class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TERMINAL_H__E454B37C_A406_4954_998B_3DCFA87D34A0__INCLUDED_)
#define AFX_TERMINAL_H__E454B37C_A406_4954_998B_3DCFA87D34A0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <atlbase.h>
#include "screen.h"
//#include "object.h"
//#include "channel.h"

#define X_LINE          1
#define X_RECT          2
#define X_ARC           3
#define X_OVAL          4
#define X_ORECT         5
#define X_RECT3D        6
#define X_IMAGE         7
#define X_LABEL         8
#define X_POLYLINE      9
#define X_POLYGON       10
#define X_SCREEN        11
#define X_EDIT          12
#define X_BUTTON        13
#define X_PANEL         14
#define X_TEXT          15
#define X_PICTURE       16

class ovec;
class object;
class channel;
class CTerminalView;
class TBitmap;
class CImage;

class Terminal  
{
	friend class screen;
	friend class CTerminalView;
	friend class channel;
	friend class Loader;
private:	// User declarations
	int cursorX,coord_x;
	int cursorY,coord_y;
	int charHeight;
	int charWidth;
	int ClientWidth;
	int ClientHeight;
	int w0;
	int h0;
	bool active;
	bool no_cursor;
	channel *channels;
	Terminal *parent;
	COLOR color[256];
	bool update[256];
	CString font_name[4];

	ovec *objects;
	int num_obhects;

	int current_screen;
	int last_screen;
	int font_std;
	int color_std;

	void	draw_cursor();
	void	draw_scar_screen();
	void	draw_image_screen();
	char	SoftFont(unsigned char);
	void	draw_box(COLOR clr,unsigned int,int,int,int,int, int atr=0);
	COLOR	create_color(int clr,int attr);
	COLOR	create_color(COLOR clr,int attr);
    COLOR	tcolor(clr *currAttr,COLOR &fg,COLOR &bg);
	CPoint	m_start;
	CPoint	m_end;
public:		// User declarations
	Terminal(channel *mf,int width, int height, CString fname, int num=0, Terminal *parent=NULL);
	virtual ~Terminal();

	void   setBounds(int w, int h);
	void   setColor(COLOR color);
	void   setFont(int);
	void   drawLine(int x0,int y0, int x1, int y1);
	void   fillRect(int x,int y, int w, int h,COLOR color);
	void   drawString(COLOR clr,CString str, int x, int y);
	void   markLine(int l, int n);
	void   SetCursorPos(int x, int y);
	int    get_cursor_pos(int l,int num);
	void   paint();
	void   Write(CString);
	void   set_color(int i,int r,int g,int b);
	void   No_Cursor(bool i);
	void   Load_Image(CString Name_Image,int x0,int y0,int x1,int y1,int transparent);
	void   Get_Image(CString, int ,int);
	void   Show_Document(CString);
	void   Show_Image(CString);
	int    Find_Last_Object(int arg);
	void   Delete_Last_Object(int arg);
	void   Delete_Object();
	bool   Delete_Object(int arg, int x0, int y0);
	void   Delete_All_Objects(int arg);
	void   Delete_Object(int i);
	void   putScreen(int x, int y, int columns, int lines, int num, int w, int h);
	void   putObject(int arg,int x0,int y0,int x1,int y1,int color,int fill,int r0,int r1,CImage *imag,CString des);
	void   putVect(int x, int y, int arg);
	void   putVect(int x, int y, int arg, CString des);
	void   set_active(bool act);
	bool   check_coord(int x,int y, int atr);
	void   putStringLen(const char *,int len);
	void   putString(CString);
	object *  get_screen();
	void  set_current_screen(int num);
	CPoint  get_coord(int x, int y);
	CPoint  image_coord(int x,int y);
	bool no_paint;
	int CharSet;
	int rus;
	screen *scr;
	TBitmap *pBitmap;
	CTerminalView *main_form;
};

#endif // !defined(AFX_TERMINAL_H__E454B37C_A406_4954_998B_3DCFA87D34A0__INCLUDED_)
