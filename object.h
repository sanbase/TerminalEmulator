// object.h: interface for the object class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OBJECT_H__A4BF85A2_7213_4AFA_AD26_8330EEC00463__INCLUDED_)
#define AFX_OBJECT_H__A4BF85A2_7213_4AFA_AD26_8330EEC00463__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "string.h"
#include "Display.h"
class CImage;

class object
{
private:
	void init(int arg,int x0,int y0,int x1, int y1, int color, bool fill, int r0, int r1, CImage *image, CString descr);
public:
	int arg;
	int x0,y0,x1,y1,r0,r1;
	int color;
	bool fill; 

	CImage  *img;
    CWnd *comp;

	CString description;
//	double p;
    CPoint *v;
    int num_v;
	Terminal *term;

	object(int arg,int x0,int y0,int x1, int y1, int color, bool fill, int r0, int r1, CImage *image, CString descr);
	object(int x, int y, int columns, int lines, CString font, int num, int w, int h, Terminal *parent);
    virtual ~object();
	void	Add_Vector(int x,int y);
	void	Free_Vector();
};

class ovec
{
private:
	object **obj;
	int num;
public:
	ovec();
	~ovec();
	int Add(object *o);
	void Del(int i);
	object *Get(int i);
	int size();
};


#endif // !defined(AFX_OBJECT_H__A4BF85A2_7213_4AFA_AD26_8330EEC00463__INCLUDED_)
