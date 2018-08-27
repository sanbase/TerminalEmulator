// object.cpp: implementation of the object class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "object.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#include "Terminal.h"
#include "TBitmap.h"
#include "Cimage.h"
#include "telnetIO.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ovec::ovec()
{
        num=0;
        obj=NULL;
}

ovec::~ovec()
{
	for(int i=0;i<num;i++)
		delete obj[i];
    if(obj!=NULL)
		free(obj);
}

int ovec::Add(object *o)
{
        obj=(object **)realloc(obj,++num*sizeof (object *));
        obj[num-1]=o;
        return(num-1);
}

void ovec::Del(int i)
{
        if(i<0 || i>=num)
                return;
        delete obj[i];
        if(num>1)
                memcpy(obj+i,obj+i+1,(num-i-1)*sizeof (object *));
        num--;
}

object *ovec::Get(int i)
{
        if(i<0 || i>=num)
                return NULL;
        return(obj[i]);
}

int ovec::size()
{
        return num;
}

void   object::init(int arg,int x0,int y0,int x1, int y1, int color, bool fill, int r0, int r1, CImage *image, CString descr)
{
        this->arg=arg;
        this->x0=x0;
        this->y0=y0;
        this->x1=x1;
        this->y1=y1;
        this->color=color;
        this->fill=fill;
        this->r0=r0;
        this->r1=r1;
        this->img=image;
        this->description=descr;
        comp=NULL;
        term=NULL;
}

object::object(int arg,int x0,int y0,int x1, int y1, int color, bool fill, int r0, int r1, CImage *image, CString descr)
{
        init(arg,x0,y0,x1,y1,color,fill,r0,r1,image,descr);
        num_v=0;
        v=NULL;
//        this->p=0;
}

object::object(int x, int y, int columns, int lines, CString font, int num, int w, int h, Terminal *parent)
{
//        this->p=0;
        init(X_SCREEN,x, y, columns,lines,num,false,w,h,NULL,font);
        num_v=0;
        v=NULL;
        term = new Terminal(NULL,lines,columns,font,num,parent);
		term->pBitmap = new TBitmap(parent->pBitmap->Canvas,w,h);
}

extern ImageHash *image_hash;
extern int num_images;

object::~object()
{
        if(term!=NULL)
                delete term;
        Free_Vector();
        if(img!=NULL)
		{
			for(int i=0;i<num_images;i++)
			{
				if(image_hash[i].image==img)
				{
					return;
				}
			}
			delete img;
		}
}

void  object::Free_Vector()
{
        if(v!=NULL)
                free(v);
        num_v=0;
}

void   object::Add_Vector(int x,int y)
{
        v=(CPoint *)realloc(v,++num_v*sizeof CPoint);
        v[num_v-1].x=x;
        v[num_v-1].y=y;

        if(x0>x)
                x0=x;
        if(y0>y)
                y0=y;
        if(x1<x)
                x1=x;
        if(y1<y)
                y1=y;
}
