// Terminal.cpp: implementation of the Terminal class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#include "Display.h"
#include "channel.h"
#include "screen.h"
#include "object.h"
#include "terminalView.h"
//#include "TImage.h"
#include "TBitmap.h"
#include "telnetIO.h"
#include <wingdi.h>
#include <math.h>
#include "CImage.h"

extern int __fastcall checkBounds(int value, int lower, int upper);
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//SOCKET Open_Socket(String address, int port);

SOCKET Open_Socket(CString address, int port);
static COLOR tmp[]=
{
	COLOR(  0,  0,  0),         // 0
	COLOR(  0, 64,192),
	COLOR(  0,184,  0),
	COLOR(  0,176,176),
	COLOR(160,  0, 96),
	COLOR( 90, 70,150),
	COLOR( 33,100,130),
	COLOR(176,192,192),

	COLOR( 56, 60,106),         // 8
	COLOR(152,152,255),
	COLOR(128,255,128),
	COLOR(128,255,255),
	COLOR(255,128,128),
	COLOR(240,176, 90),
	COLOR(255,220,120),
	COLOR(255,255,255),

	COLOR( 32, 64, 64),         // 16
	COLOR( 48, 96, 96),
	COLOR( 64,128,128),
	COLOR( 64,160,160),
	COLOR(120,160,180),
	COLOR(108,140,160),
	COLOR( 88,120,140),
	COLOR( 80,100,120),

	COLOR( 80, 80,100),         // 24
	COLOR( 60, 50,100),
	COLOR( 10, 70,100),
	COLOR( 10, 80,115),
	COLOR( 10,120,125),
	COLOR( 10,140,160),
	COLOR(130,200,190),
	COLOR(150,190,210),

	COLOR(0xff2040),            // 32 red
	COLOR(0xff7000),
	COLOR(0xf88860),
	COLOR(0xffb070),

	COLOR(0xe09858),            // 36 yellow
	COLOR(0xf8c068),
	COLOR(0xfad2a2),
	COLOR(0xe0c880),

	COLOR(0xc0c880),            // 40 green
	COLOR(0xc8d0b8),
	COLOR(0xa0c880),
	COLOR(0x90d868),

	COLOR(0xb4fac8),            // 44 green
	COLOR(0x90d0a0),
	COLOR(0x90d0c8),
	COLOR(0x30d0c8),

	COLOR(0x30b0c8),
	COLOR(0x10a0c8),            // 48 purple
	COLOR(0x7880c8),
	COLOR(0x9070c8),

	COLOR(0x806078),            // 52 brown
	COLOR(0x706060),
	COLOR(0x686860),
	COLOR(0x586848),

	COLOR(0x306848),            // 56
	COLOR(0x306860),
	COLOR(0x386870),
	COLOR(0x186888),

	COLOR(0x484848),            // 60
	COLOR(0x504010),
	COLOR(0x604020),
	COLOR(0x683000),
};

//---------------------------------------------------------------------------

Terminal::Terminal(channel *mf, int width, int height, CString fname, int num, Terminal *pnt)
{
	if(mf!=NULL)
	{
		channels=mf;
		main_form=mf->main_form;
		pBitmap = new TBitmap(mf->main_form);
	}
	else if(pnt!=NULL)
	{
		channels=pnt->channels;
		main_form=pnt->main_form;
	}
	objects=new ovec();
	parent=pnt;
	memset(color,0,sizeof color);
	memcpy(color,tmp,sizeof tmp);
	cursorX=0;
	cursorY=0;
	coord_x=0;
	coord_y=0;
	w0=width;
	h0=height;
	no_cursor=false;
	update[0]=true;
	scr=NULL;
	rus=866;
	CharSet=RUSSIAN_CHARSET;

	current_screen=0;
	last_screen=0;
	no_paint=false;
//	Lock = new CComCriticalSection;
}

Terminal::~Terminal()
{
	delete pBitmap;
	delete scr;
//	delete Lock;
	delete objects;
	pBitmap=0;
	scr=0;
}

// ----------------------------------- Objects -------------------------

void  Terminal::Show_Document(CString name)
{
	char url[256];
	if(!strncmp(name,"http://",7))
	{
		ShellExecute(0,"open",name,NULL,NULL,SW_SHOWDEFAULT);
		return;
	}
	if(channels->host!="127.0.0.1" && channels->host!="127.1" && channels->host!="localhost")
	{
		sprintf(url,"http://%s/%s",channels->host,name);
		ShellExecute(0,"open",url,NULL,NULL,SW_SHOWDEFAULT);
	}
	else
	{
		sprintf(url,"file://%s",name);
		ShellExecute(0,"open",url,NULL,NULL,SW_SHOWDEFAULT);
		Sleep(1000);
		unlink(url);
		channels->Write((char)2);
	}
}

void  Terminal::Show_Image(CString name)
{
	Show_Document(name);
}

void   Terminal::Get_Image(CString name,int x0, int y0)
{
	Load_Image(name,x0*charWidth,y0*charHeight,0,0,1);
}

ImageHash *image_hash;
int num_images;

void   Terminal::Load_Image(CString Name_Image,int x0,int y0,int x1,int y1,int transparent)
{
	CString name = Name_Image;
	CString des;
	for(int i=0;i<Name_Image.GetLength();i++)
	{
		if(Name_Image[i]=='=')
		{
			CString str = CString(Name_Image);
			name= str.Left(i);
			des = str.Right(str.GetLength()-i-1);
			break;
		}
	}
	if(x0!=-1)
	{
		for(int i=0;i<num_images;i++)
		{
			if(name==*image_hash[i].name)
			{
				putObject(X_IMAGE,x0,y0,x1,y1,transparent,0,0,0,image_hash[i].image,des);
				return;
			}
		}
	}
	Loader *l=new Loader(this,name,des,x0,y0,x1,y1,transparent);
	l->Run(pBitmap->Canvas);
	delete l;
}

int   Terminal::Find_Last_Object(int arg)
{
	for(int i=objects->size()-1;i>=0;i--)
	{
		if(objects->Get(i)->arg==arg)
		{
			return(i);
		}
	}
	return(-1);
}

void   Terminal::Delete_Last_Object(int arg)
{
	int i;
	if((i=Find_Last_Object(arg))>=0)
		Delete_Object(i);
}

void   Terminal::Delete_Object()
{
	for(int i=0;i<(int)objects->size();)
		Delete_Object(i);
//        del_choice();
}

bool   Terminal::Delete_Object(int arg, int x0, int y0)
{
	for(int i=0;i<(int)objects->size();i++)
	{
		object *obj=objects->Get(i);
		if(obj->arg==arg && obj->x0==x0 && obj->y0==y0)
		{
			Delete_Object(i);
			return true;
		}
	}
	return false;
}

void   Terminal::Delete_All_Objects(int arg)
{
	for(int i=0;i<(int)objects->size();)
	{
		if(objects->Get(i)->arg==arg || arg==0)
			Delete_Object(i);
		else  i++;
	}
	return;
}

void   Terminal::Delete_Object(int i)
{
	if(i>(int)objects->size())
		return;
	object *obj=objects->Get(i);
//        main_form->Del_Object(obj,channels->act);
	objects->Del(i);
	update[0]=true;
}

void   Terminal::putScreen(int x, int y, int columns, int lines, int num, int w, int h)
{
	if(w==0 || h==0 || columns==0 || lines==0)
		return;

	Delete_Object(X_SCREEN,x,y);
	for(int i=0;i<(int)objects->size();)
	{
		object *obj=objects->Get(i);
		if(obj->arg==X_SCREEN &&  obj->color==num)
			Delete_Object(i);
		else i++;
	}
	object *obj = new object(x,y,columns,lines,font_name[0],num,w*charWidth,h*charHeight,parent==NULL?this:parent);
	obj->term->setBounds(w*charWidth,h*charHeight);
	obj->term->channels=channels;
	obj->term->set_active(active);
	obj->term->paint();
	objects->Add(obj);

//        repaint();
}

void   Terminal::putObject(int arg,int x0,int y0,int x1,int y1,int color,int fill,int r0,int r1,CImage *imag,CString des)
{
	if(arg==0)
		return;

	if(arg==X_POLYLINE || arg==X_POLYGON)
	{
		putVect(x0,y0,arg,des);
		return;
	}
	if(arg==X_IMAGE || arg==X_EDIT || arg==X_BUTTON || arg==X_PANEL)
		Delete_Object(arg,x0,y0);
	if(x0==0 && x1==0 && y0==0 && y1==0)
	{
		if(arg==X_RECT || arg==X_RECT3D || arg==X_ORECT || arg==X_LABEL || arg==X_OVAL)
		{
			Delete_All_Objects(arg);
			return;
		}
		if(arg==X_LINE)
		{
			Delete_All_Objects(X_LINE);
			Delete_All_Objects(X_POLYLINE);
			return;
		}
	}
	else
	{
		if(arg==X_RECT || arg==X_RECT3D || arg==X_ORECT || arg==X_LABEL || arg==X_OVAL)
		{
			markLine(y0/charHeight,(y1/charHeight+1));
		}
		if(arg==X_LINE)
		{
			int y,n;
			if(y0>y1)
			{
				y=y1;
				n=y0-y1;
			}
			else
			{
				y=y0;
				n=y1-y0;
			}
			markLine(y/charHeight,(n/charHeight+1));
		}
		if(arg==X_IMAGE)
		{
//                        markLine(y0/charHeight,((y1==0?imag->Height+charHeight:y1)/charHeight+1));
			update[0]=true;
		}
	}
	object *obj=new object(arg,x0,y0,x1,y1,color,fill>0,r0,r1,imag,des);
	objects->Add(obj);
//        main_form->Add_Object(obj,channels->act);
}

void   Terminal::putVect(int x, int y, int arg)
{
	putVect(x,y,arg,"");
}

void   Terminal::putVect(int x, int y, int arg, CString des)
{
	int vec;
	if(arg==X_POLYLINE)
		vec=Find_Last_Object(X_POLYLINE);
	else
		vec=Find_Last_Object(X_POLYGON);
	int num=Find_Last_Object(X_LINE);
	if(vec>=0 && vec>num)
	{
		object *obj=objects->Get(vec);
		obj->Add_Vector(x,y);
		return;
	}
	if(num>=0)
	{
		object *obj=objects->Get(num);
		int x0=obj->x0;
		int y0=obj->y0;
		int x1=obj->x1;
		int y1=obj->y1;
		obj->arg=arg==X_POLYLINE?X_POLYLINE:X_POLYGON;
		obj->Add_Vector(x0,y0);
		obj->Add_Vector(x1,y1);
		obj->Add_Vector(x,y);
	}
	else
	{
		object *obj=new object(X_LINE,x,y,x,y,14,false,0,0,NULL,des);
		objects->Add(obj);
	}
}

//---------------------------------------------------------------------------
void   Terminal::setBounds(int w, int h)
{
	int Char_Length[256];

	ClientWidth=w;
	ClientHeight=h;
	font_name[0]="Courier New";
	font_name[1]="Times New Roman";
	font_name[2]="MS Serif";
	font_name[3]="Garamond";

	charHeight=h/h0;

	pBitmap->SetClientFont(font_name[0],charHeight,w,h,false,false,CharSet);
	font_std=0;
	pBitmap->Canvas->GetOutputCharWidth(0,255,Char_Length);
	charWidth=Char_Length['W'];
	screen *tmp=NULL;
	if(scr!=NULL)
		tmp=scr;
	scr = new screen(h0=h/charHeight,w0=w/charWidth,this);
	if(tmp!=NULL)
	{
		scr->copy(tmp);
		delete tmp;
	}
}

void   Terminal::set_color(int i,int r,int g,int b)
{
	if(i<64 || i>=128)
		return;
	color[i]=COLOR(r,g,b);
}

void   Terminal::No_Cursor(bool i)
{
	no_cursor=i;
	markLine(cursorY, 1);
}

void   Terminal::setColor(COLOR color)
{
	if(color_std==(int)color.value())
		return;
	color_std=color.value();
	pBitmap->SetPen(color_std);
}

void   Terminal::setFont(int font)
{
	if(font==font_std)
		return;
	bool italic=(font&1)>0;
	bool bold=  (font&2)>0;

	pBitmap->SetClientFont(font_name[(font>>2)%4],charHeight,ClientWidth, ClientHeight, bold,italic,CharSet);
	font_std=font;
}

void   Terminal::drawLine(int x0, int y0, int x1, int y1)
{
	pBitmap->Canvas->MoveTo(x0,y0);
	pBitmap->Canvas->LineTo(x1,y1);
}

void   Terminal::SetCursorPos(int c, int l)
{
	c = checkBounds(c, 0, scr->Width() - 1);
	l = checkBounds(l, 0, scr->Height() - 1);
	markLine(cursorY, 1);
	cursorX = (c < scr->Width() ? c : scr->Width());
	cursorY = (l < scr->Height() ? l : scr->Height());
	markLine(cursorY, 1);
}

void   Terminal::fillRect(int x, int y, int w, int h,COLOR color)
{
	CRect rec(x,y,x+w,y+h);
	pBitmap->FillRect(&rec,color.value());
}

void   Terminal::drawString(COLOR clr,CString str, int x, int y)
{

	CString buf;
	unsigned char *ch=(unsigned char *)(const char *)str;
	for(int i=0;i<str.GetLength();i++)
	{
		if(ch[i]>127)
		{
			char uc=SoftFont((unsigned char)ch[i]);
			if(uc!=0)
				buf+=uc;
			else    buf+=" ";
		}
		else    buf+=ch[i];
	}
	if((clr.r+clr.g+clr.b)>200 && charHeight>16)
	{
// shadow of text
		pBitmap->Canvas->SetTextColor(0);
		pBitmap->Canvas->TextOut(x+1,y+1,buf);
		pBitmap->Canvas->TextOut(x+1,y,buf);
	}
	pBitmap->Canvas->SetTextColor(clr.value());
	pBitmap->Canvas->TextOut(x,y,buf);
}

void   Terminal::markLine(int l, int n)
{
	l = checkBounds(l, 0, scr->Height() - 1);
	for(int i = 0; i < n && l + i <= scr->Height(); i++)
	{
		if(parent!=NULL)
		{
			for(int i=0;i<parent->objects->size();i++)
			{
				object *obj=parent->objects->Get(i);
				if(obj->term==this)
				{
					int y0=obj->y0;
					parent->update[(y0+(l+i)*charHeight)/parent->charHeight+1]=true;
					break;
				}
			}
		}
		update[l + i + 1] = true;
	}
}

//---------------------------------------------------------------------------
COLOR Terminal::create_color(COLOR clr,int attr)
{
	double r_c = clr.getRed();
	double g_c = clr.getGreen();
	double b_c = clr.getBlue();
	double k1=1.5;
	double k2=attr==6?1.7:1.5;
	double k3=attr==5?1.2:1.5;
	switch(attr)
	{
	case 1:
		return(COLOR((int)r_c,(int)g_c,(int)b_c));
	case 4:
		k3=2.0;
	case 2: // light
		r_c/=k1;
		g_c/=k1;
		b_c/=k1;
		r_c+=30;
		g_c+=30;
		b_c+=30;
		r_c*=k3;
		g_c*=k3;
		b_c*=k3;
		if(r_c>255)
			r_c=255;
		if(g_c>255)
			g_c=255;
		if(b_c>255)
			b_c=255;
		if(r_c<0)
			r_c=0;
		if(g_c<0)
			g_c=0;
		if(b_c<0)
			b_c=0;
		return(COLOR((int)r_c,(int)g_c,(int)b_c));
	case 5:	// light
		if(r_c>200 || b_c>200 || g_c>200)
		{
			r_c-=70;
			g_c-=70;
			b_c-=70;
			k3=1.7;
		}
		r_c*=k3;
		g_c*=k3;
		b_c*=k3;
		if(r_c>255)
			r_c=255;
		if(g_c>255)
			g_c=255;
		if(b_c>255)
			b_c=255;
		if(r_c<0)
			r_c=0;
		if(g_c<0)
			g_c=0;
		if(b_c<0)
			b_c=0;
		return(COLOR((int)r_c,(int)g_c,(int)b_c));
	case 3: // dark
		r_c-=25;
		g_c-=20;
		b_c-=15;
		r_c/=k2;
		g_c/=k2;
		b_c/=k2;
		if(r_c<0)
			r_c=0;
		if(g_c<0)
			g_c=0;
		if(b_c<0)
			b_c=0;
		return(COLOR((int)r_c,(int)g_c,(int)b_c));
	case 6:
		if(r_c>g_c)
			g_c/=1.3;
		if(r_c>b_c)
			b_c/=1.3;
		if(b_c>g_c)
			g_c/=1.3;
		if(b_c>r_c)
			r_c/=1.3;
		if(g_c>r_c)
			r_c/=1.3;
		if(g_c>b_c)
			b_c/=1.3;
		r_c/=k2;
		g_c/=k2;
		b_c/=k2;
		return(COLOR((int)r_c,(int)g_c,(int)b_c));
	default:
		return(clr);
	}
}

COLOR    Terminal::create_color(int clr,int attr)
{
	int col=clr&0x3f;
	int r_c = color[col].getRed();
	int g_c = color[col].getGreen();
	int b_c = color[col].getBlue();

	if(clr>63 && clr<128)
	{
		r_c=b_c=g_c=col*4;

		if(attr==0)
			attr=1;
	}
	else
	{
		if(clr>=128 && clr<192 && attr==0)
			attr=2;
		if(clr>=192 && attr==0)
			attr=3;
	}
	return(create_color(COLOR(r_c,g_c,b_c),attr));

}
//---------------------------------------------------------------------------
void Terminal::paint()
{
	if(active==false || no_paint)
	{
		return;
	}

	
	draw_scar_screen();
	draw_image_screen();
	draw_cursor();

//	SetDIBits(pBitmap->Canvas->m_hDC,(HBITMAP)pBitmap->out_bmp->GetSafeHandle(),0,pBitmap->Height,pBitmap->buffer,&pBitmap->bmi,DIB_RGB_COLORS);
//	pBitmap->Canvas->SelectObject(pBitmap->m_bmp);
}

COLOR  Terminal::tcolor(clr *currAttr,COLOR &fg,COLOR &bg)
{
	fg=create_color(currAttr->fg,currAttr->fg>>6);
	bg=create_color(currAttr->bg,currAttr->bg>>6);

	if(currAttr->fg==0 && currAttr->bg==0)
	{
		fg = color[7];
		bg = color[0];
	}

	if(currAttr->bwlt!=0)
	{
		bg=COLOR((bg.getRed()/3)+64,(bg.getGreen()/3)+64,(bg.getBlue()/3)+64);
		fg=COLOR((fg.getRed()/3)+64,(fg.getGreen()/3)+64,(fg.getBlue()/3)+64);
	}
	if(fg.equals(bg))
		fg=color[0];

	int r_c=bg.getRed();
	int g_c=bg.getGreen();
	int b_c=bg.getBlue();

	if((r_c+g_c+b_c)>300 && currAttr->x1)
	{
		int a1,a2,a3;

		a1=(int)(r_c*0.8);
		a2=(int)(g_c*0.8);
		a3=(int)(b_c*0.8);
		bg=COLOR(a1,a2,a3);
	}
	double a=1.4;
	r_c=(int)(r_c*a);
	g_c=(int)(g_c*a);
	b_c=(int)(b_c*a);
	if(r_c>255)
		r_c=255;
	if(g_c>255)
		g_c=255;
	if(b_c>255)
		b_c=255;
	return(COLOR(r_c,g_c,b_c));
}

static unsigned char koi8(unsigned char ch)
{
	static int tabl[]=
	{
		196,179,218,191,192,217,195,180,194,193,197,220,223,219,221,222,
		176,177,178,186,170,180,165,191,175,000,000,000,000,000,000,000,
		205,186,213,241,214,201,184,210,187,212,211,200,190,198,188,189,
		199,204,181,240,182,185,209,210,203,207,208,202,216,215,206,169,
		238,160,161,230,164,165,228,163,229,168,169,170,171,172,173,174,
		175,239,224,225,226,227,166,162,236,235,167,232,237,233,231,234,
		158,128,129,150,132,133,148,131,149,136,137,138,139,140,141,142,
		143,159,144,145,146,147,134,130,156,235,135,152,157,153,151,154	};
	if(ch>=128)
		return(tabl[ch-128]);
	return(ch);
}

//---------------------------------------------------------------------------
void   Terminal::draw_scar_screen()
{
	COLOR fg = color[7];
	COLOR bg = color[0];
	if(m_start!=m_end)
		update[0]=true;
	channels->update[0]|=update[0];
	for(int l = 0; l < scr->Height(); l++)
	{
		if(update[0]==false && update[l + 1]==false)
			continue;
		channels->update[l+1]|=update[l+1];
		update[l + 1] = false;

		for(int c = 0; c < scr->Width();) // draw rectangle area
		{
			int addr = 0;
			clr currAttr = scr->Attr(l,c);
			int x=c+1;
			int y=l+1;
			int flg=0;
			if(m_start!=m_end && y>=m_start.y && y<=m_end.y)
			{
				flg=1;
				if(x>=m_start.x && x<m_end.x)
				{
					if(y>=m_start.y && y<=m_end.y)
						currAttr.bg=96;
				}
				else if(y>m_start.y && y<=m_end.y && x<=m_start.x)
					currAttr.bg=96;
				else if(y>=m_start.y && y<m_end.y && x>m_start.x)
					currAttr.bg=96;
			}
			tcolor(&currAttr,fg,bg);

// determine the maximum of characters we can print in one go
			while (c + addr < scr->Width() && scr->Attr(l,c + addr).bg == currAttr.bg && scr->Attr(l,c + addr).bwlt==currAttr.bwlt && flg==0)
			{
				addr++;
			}
			if(addr==0)
				addr=1;

// clear the part of the screen we want to change (fill rectangle)
			fillRect(c*charWidth, l*charHeight, addr*charWidth, charHeight,bg);
			c+=addr;
		}

		for(c = 0; c < scr->Width();)   // draw text
		{
			int addr = 0;

			clr currAttr = scr->Attr(l,c);
			tcolor(&currAttr,fg,bg);

// determine the maximum of characters we can print in one go
			while (c + addr < scr->Width()
			    && scr->Attr(l,c + addr).bg == currAttr.bg
			    && scr->Attr(l,c + addr).fg == currAttr.fg
			    && scr->Attr(l,c + addr).font == currAttr.font
			    && SoftFont(scr->Value(l,c + addr)))
			{
				if (scr->Value(l,c + addr) == 0)
					scr->Set_Char(c + addr,l,' ');
				addr++;
			}
			if(addr==0)
				addr=1;

			if(!SoftFont(scr->Value(l,c)))
			{
				COLOR clr=fg;
				if(fg.equals(color[0]) || (currAttr.fg==currAttr.bg))
					clr=create_color(bg,2);
				if(fg.equals(color[15]))
					clr=color[7];
				for(int j=0;j<addr;j++)
				{
					unsigned char ch=scr->Value(l,c+j);
					if(ch==0)
						continue;
					if(rus==8)
						ch=koi8(ch);

					draw_box(clr,ch,(c+j)*charWidth,l*charHeight,charWidth,charHeight,currAttr.fg==currAttr.bg?2:0);
				}
			}
			else
			{
				setFont(currAttr.font);
// draw the characters

				for(int i=0;i<addr;i++)
				{
					int j=addr;
					if(((currAttr.font>>2)&0x4)==0)
					{
						while(i<addr && scr->Value(l,c+i)==' ')
							i++;
						for(j=i;j<addr;j++)
						{
							if(scr->Value(l,c+j)==' ')
								break;
						}
					}
					CString str=scr->Chars(l,c+i, j-i);
					drawString(fg, str, (c+i)*charWidth, (l)*charHeight);
					i=j-1;
				}
			}
			c += addr;
		}
		for(c = 0; c < scr->Width(); c++)   // draw shadows
		{
			clr currAttr = scr->Attr(l,c);

			if((currAttr.frame)!=0)
			{
				int i=currAttr.frame&0xf;
				COLOR tc=tcolor(&currAttr,fg,bg);
				int r_c=tc.getRed();
				int g_c=tc.getGreen();
				int b_c=tc.getBlue();

				int x0=c*charWidth;
				int y0=l*charHeight;
				int l0=charWidth;
				int nap=currAttr.frame&0x10;

				if((i&0x1)!=0) // - up
				{
					if(nap)
						setColor(COLOR(r_c>>1,g_c>>1,b_c>>1));
					else
						setColor(COLOR(r_c,g_c,b_c));
					drawLine(x0,y0,x0+l0,y0);
					drawLine(x0,y0+1,x0+l0,y0+1);
				}
				if((i&0x2)!=0) // _ down
				{
					if(nap)
						setColor(COLOR(r_c,g_c,b_c));
					else
						setColor(COLOR(r_c>>1,g_c>>1,b_c>>1));
					drawLine(x0,y0+charHeight-2,x0+l0,y0+charHeight-2);
					drawLine(x0,y0+charHeight-1,x0+l0,y0+charHeight-1);
					drawLine(x0,y0+charHeight,x0+l0,y0+charHeight);
				}
				if((i&0x4)!=0) // | left
				{
					if(nap)
						setColor(COLOR(r_c>>1,g_c>>1,b_c>>1));
					else
						setColor(COLOR(r_c,g_c,b_c));
					drawLine(x0,y0,x0,y0+charHeight);
					drawLine(x0+1,y0+((i&0x1)!=0?1:0),x0+1,y0+charHeight+((i&0x2)!=0?-1:0));
				}
				if((i&0x8)!=0) // | right
				{
					if(nap)
						setColor(COLOR(r_c,g_c,b_c));
					else
						setColor(COLOR(r_c>>1,g_c>>1,b_c>>1));
					drawLine(x0+l0-1,y0,x0+l0-1,y0+charHeight);
					drawLine(x0+l0-2,y0+((i&0x1)!=0?1:0),x0+l0-2,y0+charHeight+((i&0x2)!=0?-1:-0));
				}
			}

			if(currAttr.x1 != 0)
			{
				tcolor(&currAttr,fg,bg);
				fg=COLOR(bg.getRed()>>1,bg.getGreen()>>1,bg.getBlue()>>1);
				bg=create_color(bg,4);

				if(currAttr.bwlt!=0)
				{
					int a1;

					a1=(int)(bg.getRed()*0.3)+(int)(bg.getGreen()*0.3)+(int)(bg.getBlue()*0.3);
					bg = COLOR((int)(0.8*a1),(int)(0.9*a1),a1);

					a1=(int)(fg.getRed()*0.3)+(int)(fg.getGreen()*0.3)+(int)(fg.getBlue()*0.3);
					fg=COLOR((int)(0.8*a1),(int)(0.9*a1),a1);
				}
				if(currAttr.x1==1)
					setColor(fg);
				else  setColor(bg);

				if(c>=scr->Width() || (currAttr.bg)!=(scr->Attr(l,c).bg))
				{
					drawLine(c * charWidth  - 1 + charWidth,
					(l)* charHeight,
					c * charWidth  - 1 + charWidth,
					(l+1)* charHeight - 1);
				}
				drawLine(c*charWidth,(l+1)*charHeight-1,c*charWidth+charWidth,(l+1)*charHeight-1);

				if(currAttr.x1==1)
					setColor(bg);
				else
					setColor(fg);

				if(c==0 || currAttr.bg!=scr->Attr(l,c-1).bg)
					drawLine(c*charWidth,(l)*charHeight,c*charWidth,(l+1)*charHeight-1);
				drawLine(c*charWidth,(l)*charHeight,c*charWidth+charWidth,(l)*charHeight);
				if(currAttr.bg!=scr->Attr(l,c+1).bg)
				{
					if(currAttr.x1==1)
						setColor(fg);
					else
						setColor(bg);
					drawLine((c+1)*charWidth-1,(l)*charHeight,(c+1)*charWidth-1,(l+1)*charHeight-1);
				}
			}
		}

	}
	update[0]=false;
}
//---------------------------------------------------------------------------
void   Terminal::draw_image_screen()
{
	int act_image=-1;
	for(int num=0;num<(int)objects->size();num++)
	{
		object *obj = objects->Get(num);

		if(obj->arg==X_IMAGE)
		{
			if(obj->img!=NULL)
			{
				BITMAP bm;
				obj->img->GetBitmap(&bm);

				int l=bm.bmWidth;
				int h=bm.bmHeight;
				if(obj->fill && obj->description.GetLength())
					act_image=num;
				if(l>0 && h>0)
				{
					if(obj->x1==0)
						obj->x1=l;
					if(obj->y1==0)
						obj->y1=h;

					double p=0;

					if(obj->y1!=l)
					{
						p=(double)obj->x1/l;
						l=(int)(l*p);
					}
					if(obj->y1!=h)
					{
						p=(double)obj->y1/h;
						h=(int)(h*p);
					}
					if(obj->color)
					{
/*
						CPoint pt;
						pt.x=obj->x0;
						pt.y=obj->y0;
						if(p!=0)
						{
							CSize size;
							size.cx=l;
							size.cy=h;
							pBitmap->Canvas->DrawState(pt,size,obj->img,DST_BITMAP | DSS_NORMAL); 
						}
						else
						{
							pBitmap->Canvas->DrawState(pt,0,obj->img,DST_BITMAP); 
						}

*/
						CDC dcImage;
						dcImage.CreateCompatibleDC(pBitmap->Canvas);
						CBitmap* pOldBitmapImage = dcImage.SelectObject(obj->img);
						if(p!=0)
							pBitmap->Canvas->StretchBlt(obj->x0,obj->y0,l,h,&dcImage,0,0,bm.bmWidth,bm.bmHeight,SRCCOPY);
						else
							pBitmap->Canvas->BitBlt(obj->x0,obj->y0,bm.bmWidth,bm.bmHeight,&dcImage,0,0,SRCCOPY);
						dcImage.SelectObject(pOldBitmapImage);
						dcImage.DeleteDC();

					}
					else
					{

						CDC dcImage, dctrans ;

						dctrans.CreateCompatibleDC(pBitmap->Canvas) ;
						dcImage.CreateCompatibleDC(pBitmap->Canvas);

						BITMAP bitstruct ;
						obj->img->GetBitmap ( &bitstruct) ;
						int width = bitstruct.bmWidth;
						int height = bitstruct.bmHeight;

						CBitmap transbitmap ;
						transbitmap.CreateBitmap ( width, height, 1, 1, NULL) ;

						CBitmap *pOldImageBitmap = dcImage.SelectObject (obj->img);
						CBitmap *pOldTransBitmap = dctrans.SelectObject (&transbitmap);

						dcImage.SetBkColor (dcImage.GetPixel(0,0));
						dctrans.BitBlt ( 0, 0, width, height, &dcImage, 0, 0, SRCCOPY );

						pBitmap->Canvas ->SetTextColor(0);
						pBitmap->Canvas -> BitBlt (obj->x0,obj->y0,width,height,&dcImage, 0, 0, SRCINVERT );
						pBitmap->Canvas -> BitBlt (obj->x0,obj->y0,width,height,&dctrans, 0, 0, SRCAND );
						pBitmap->Canvas -> BitBlt (obj->x0,obj->y0,width,height,&dcImage, 0, 0, SRCINVERT );

						dctrans.SelectObject (pOldTransBitmap) ;

						dcImage.SelectObject(pOldImageBitmap);
						dcImage.DeleteDC();

						dctrans.DeleteDC();
					}
				}

			}
		}
		if((obj->arg==X_POLYLINE || obj->arg==X_POLYGON) && obj->num_v>0)
		{
			int size=obj->num_v;
			if(size>0)
			{
				setColor(color[obj->color]);
				if(obj->arg==X_POLYLINE)
					pBitmap->Canvas->Polyline(obj->v,size);
				else
				{
					if(obj->fill)
					{
						pBitmap->SetBrush(color[obj->color].value());
						pBitmap->Canvas->Polygon(obj->v,size);
					}
					else
						pBitmap->Canvas->Polygon(obj->v,size);
				}
			}
		}
		if(obj->arg==X_LINE)
		{
			setColor(color[obj->color]);
			drawLine(obj->x0,obj->y0,obj->x1,obj->y1);
		}
		if(obj->arg==X_ORECT)
		{
			setColor(color[obj->color]);
			if(obj->fill)
			{
				pBitmap->SetBrush(color[obj->color].value());
				pBitmap->Canvas->RoundRect(obj->x0,obj->y0,obj->x1,obj->y1,obj->r0,obj->r1);
			}
			else
				pBitmap->Canvas->RoundRect(obj->x0,obj->y0,obj->x1,obj->y1,obj->r0,obj->r1);
		}
		if(obj->arg==X_RECT3D || obj->arg==X_RECT)
		{
			setColor(color[obj->color]);
			if(obj->fill)
			{
				pBitmap->Canvas->FillSolidRect(obj->x0,obj->y0,obj->x1,obj->y1,color[obj->color].value());
			}
			else
			{
				CBrush Brush;
				Brush.CreateSolidBrush(color[obj->color].value());
				pBitmap->Canvas->FrameRect(CRect(obj->x0,obj->y0,obj->x0+obj->x1,obj->y0+obj->y1),&Brush);
			}
			if(obj->arg==X_RECT3D)
			{
				pBitmap->SetBrush(color[obj->color].value());
				pBitmap->Canvas->Draw3dRect(obj->x0,obj->y0,obj->x1,obj->y1,create_color(color[obj->color],2).value(),create_color(color[obj->color],3).value());
			}
		}
		if(obj->arg==X_OVAL)
		{
			setColor(color[obj->color]);
			if(obj->fill)
			{
				pBitmap->SetBrush(color[obj->color].value());
				pBitmap->Canvas->Ellipse(obj->x0,obj->y0,obj->x1,obj->y1);
			}
			else
				pBitmap->Canvas->Ellipse(obj->x0,obj->y0,obj->x1,obj->y1);
		}
		if(obj->arg==X_ARC)
		{
			int x1=(int)(100*cos(-6.28*((double)obj->r0/360.0)));
			int y1=(int)(100*sin(-6.28*((double)obj->r0/360.0)));
			int x2=(int)(100*cos(-6.28*((double)(obj->r0+obj->r1)/360.0)));
			int y2=(int)(100*sin(-6.28*((double)(obj->r0+obj->r1)/360.0)));
			int cx=obj->x0+obj->x1/2;
			int cy=obj->y0+obj->y1/2;

			x1+=cx;
			x2+=cx;
			y1+=cy;
			y2+=cy;

			setColor(color[obj->color]);
			if(obj->fill)
			{
				pBitmap->SetBrush(color[obj->color].value());
				pBitmap->Canvas->Pie(obj->x0,obj->y0,obj->x0+obj->x1,obj->y0+obj->y1,x1,y1,x2,y2);
			}
			else
				pBitmap->Canvas->Arc(obj->x0,obj->y0,obj->x0+obj->x1,obj->y0+obj->y1,x1,y1,x2,y2);
		}
		if(obj->arg==X_LABEL)
		{
			COLOR fg;
			COLOR bg;
			clr currAttr;
			
			currAttr.bg=obj->r0;
			currAttr.fg=obj->color;
			currAttr.x1=0;
			currAttr.bwlt=scr->Attr(obj->x0/charWidth,obj->y0/charHeight).bwlt;

			tcolor(&currAttr,fg,bg);

			if(obj->fill)
				fillRect(obj->x0,obj->y0,obj->x1,obj->y1,bg);
//			if(obj->fill)
//				pBitmap->Canvas->DrawEdge(CRect(obj->x0,obj->y0,obj->x0+obj->x1,obj->y0+obj->y1),obj->fill==1?EDGE_RAISED:EDGE_SUNKEN,BF_RECT);

			if(obj->description.GetLength())
			{
				
				int fnt=(obj->r1/4)%4;
				int mod=obj->r1%4;
				int i,j,lines=0;
				CString str;
				CString maxstr;

				for(i=0;;i++)
				{
					if(i==obj->description.GetLength() || obj->description[i]=='\n')
					{
//						if(i!=obj->description.GetLength() || obj->description[i]!='\n')
							lines++;
						if(str.GetLength()>maxstr.GetLength())
							maxstr=str;
						if(i==obj->description.GetLength())
							break;
						str.Empty();
					}
					else	str+=obj->description[i];
				}
				str.Empty(); 

				pBitmap->SetClientFont(font_name[fnt],maxstr,obj->x1,obj->y1/lines,(mod&2)>0,(mod&1)>0,CharSet);

				if(lines==1)
				{
						CSize size=pBitmap->Canvas->GetTextExtent(obj->description);
						drawString(fg,obj->description,obj->x0+(obj->x1-size.cx)/2,obj->y0+(obj->y1-size.cy)/2);
				}
				else for(i=0,j=0;;i++)
				{
					if(i==obj->description.GetLength() || obj->description[i]=='\n')
					{
						CSize size=pBitmap->Canvas->GetTextExtent(str);
						drawString(fg,str,obj->x0,obj->y0+size.cy*j);
						if(i==obj->description.GetLength())
							break;
						str.Empty();
						j++;
					}
					else
						str+=obj->description[i];
				}
				font_std=-1;
				setFont(0);
			}
		}
		if(obj->arg==X_SCREEN)
		{
			obj->term->paint();

			if(obj->term->pBitmap!=NULL)
			{
				pBitmap->Canvas->BitBlt(obj->x0*charWidth,obj->y0*charHeight,obj->term->pBitmap->Width,obj->term->pBitmap->Height,obj->term->pBitmap->Canvas,0,0,SRCCOPY);
//                                setColor(color[15]);
//                                pBitmap->Canvas->Rectangle(obj->x0*charWidth,obj->y0*charHeight,obj->x0*charWidth+obj->r0,obj->y0*charWidth+obj->r1);
				update[obj->y0+(obj->term->cursorY*obj->term->charHeight)/charHeight+1]=true;
			}
		}
	}
	if(act_image>=0)
	{
		object *obj = objects->Get(act_image);
		if(obj->fill && obj->description.GetLength())
		{
			try
			{
				int x=obj->x0;
				int y=obj->y0;
				int w=obj->x1;

				int font_size=pBitmap->Font()->size;

				if(font_size>16)
					font_size=16;
				pBitmap->SetClientFont(font_name[0],font_size,0,0,false,false,CharSet);
				CSize size=pBitmap->Canvas->GetTextExtent(obj->description);
				int l=size.cx+2;
				if(x+w+l+charWidth>scr->Width()*charWidth)
					x-=(l+charWidth);
				else x+=w+charWidth;

				fillRect(x,y,l+3,size.cy+1,COLOR(255,255,200));
				setColor(color[0]);
				pBitmap->Canvas->Rectangle(x,y,x+l+3,y+size.cy+1);
				drawString(color[0],obj->description,x+2,y);
				font_std=-1;
				setFont(0);

			}
			catch (...)
			{
				;
			}
		}
	}
//        pBitmap->Canvas->Brush->Style=bsClear;
}


//---------------------------------------------------------------------------
int  Terminal::get_cursor_pos(int l,int num)
{
	int len=0;

	for(int c = 0; c < scr->Width() && c<num;) // draw reatangle area
	{
		int addr = 0;
		clr currAttr = scr->Attr(l,c);

		while (c + addr < scr->Width() && c + addr < num
		    && scr->Attr(l,c + addr).bg == currAttr.bg
		    && scr->Attr(l,c + addr).fg == currAttr.fg
		    && scr->Attr(l,c + addr).font == currAttr.font
		    && SoftFont(scr->Value(l,c + addr)))
		{
			addr++;
		}
		if(addr==0)
			addr=1;

		setFont(currAttr.font);
		for(int i=0;i<addr;i++)
		{
			int j=addr;
			if(((currAttr.font>>2)&0x4)==0)
			{
				while(i<addr && scr->Value(l,c+i)==' ')
				{
					i++;
				}
				for(j=i;j<addr;j++)
				{
					if(scr->Value(l,c+j)==' ')
						break;
				}
			}

			len=(c+i)*charWidth;
			for(int k=0;k<j-i;k++)
			{
				int ch_len=pBitmap->Font()->CharWidth[(unsigned char)scr->Char(c+i+k,l).ch];
				if(ch_len<=0)
				{
					ch_len=charWidth;
				}
				len+=ch_len;
			}
			i=j-1;
		}
		if(!SoftFont(scr->Value(l,c)))
		{
			len=(c+addr)*charWidth;
		}
		c += addr;
	}
	return(len);
}

void   Terminal::draw_cursor()
{
// draw cursor
	if(no_cursor==false)
	{
		if(cursorY >= 0 && cursorY < scr->Height())
		{
			clr currAttr = scr->Attr(cursorY,cursorX);
			COLOR bg=create_color(currAttr.bg,currAttr.bg>>6);
			int i=bg.getRed()+bg.getGreen()+bg.getBlue();

			int x=get_cursor_pos(cursorY,cursorX);
//			if(x<0)
//				x=charWidth*cursorX;
			int l=get_cursor_pos(cursorY,cursorX+1)-x;

			int y=(cursorY+1)*charHeight-2;
			CRect rt;
			rt.left=x;
			rt.top=y;
			rt.right=x+l;
			rt.bottom=y+2;
			fillRect(x,y,l,2,i>512?color[0]:color[15]);
		}
	}
}

void   Terminal::Write(CString str)
{
	channels->io->Send(str);
}

void convert_from(unsigned char *data,const char *table)
{
	if(table == NULL) return;
	if(*data >= 128) *data = 128 + table[*data - 128];
}

void convert_to(unsigned char *data,const char *table)
{
	int i;
	char itable[128];
	if(table == NULL) return;
	memset(itable,0,128);
	for(i = 0; i < 128; i++) itable[table[i]] = i;
	if(*data >= 128)
		*data = 128 + itable[*data - 128];
}

char   Terminal::SoftFont(unsigned char ch)
{
	if(ch>=' ' && ch<128)
		return(ch);

	switch(rus)
	{
	case 8:
		ch=koi8(ch);
	case 866:
		{
			static int tabl[]={
				168,184,175,191,170,186,165,180,171,187,153,174,169,149,136,133			};

			if(ch>=128 && ch<=175)
				return(ch+64);
			if(ch>=224 && ch<=239)
				return(ch+16);
			if(ch>=240 && ch<248)
				return tabl[ch-240];
			return(0);
		}
	default:
		return ch;
	}
	return(0);
}

CPoint  Terminal::get_coord(int x, int y)
{
	CPoint p = CPoint(-1,-1);
	object *obj=get_screen();
	if(obj!=NULL)
	{
		return(obj->term->get_coord(x-obj->x0*charWidth,y-obj->y0*charHeight));
	}
	p.x=x/charWidth+1;
	p.y=y/charHeight+1;
	return(p);
}

CPoint  Terminal::image_coord(int x,int y)
{
	object *obj=get_screen();
	if(obj!=NULL)
		return(obj->term->image_coord(x-obj->x0*charWidth,y-obj->y0*charHeight));
	CPoint p = CPoint(-1,-1);
	for (int i=0;i<objects->size();i++)
	{
		object *im = objects->Get(i);
		if(im->arg!=X_IMAGE || im->img==NULL || im->x1==0)
			continue;

		if(x>im->x0 && x<(im->x0+im->x1) && y>im->y0 && y<(im->y0+im->y1))
		{
			p.x=im->x0/charWidth+1;
			p.y=im->y0/charHeight+1;
			break;
		}
	}
	return p;
}

object *Terminal::get_screen()
{
	if(parent!=NULL)
		return NULL;
	if(current_screen==0 || objects->size()==0)
		return NULL;
	if(last_screen>=(int)objects->size() || last_screen<0)
		last_screen=0;

	object *obj=objects->Get(last_screen);
	if(obj==NULL)
		return NULL;
	if(obj->arg==X_SCREEN && obj->color==current_screen)
		return(obj);
	else
	{
		for(last_screen=0;last_screen<(int)objects->size();last_screen++)
		{
			obj=objects->Get(last_screen);
			if(obj->arg==X_SCREEN &&  obj->color==current_screen)
				return obj;
		}
	}
	last_screen=0;
	return NULL;
}

bool Terminal::check_coord(int x,int y, int atr)
{
	bool ret=false;

	if(parent==NULL)
	{
		object *obj=get_screen();
		if(obj!=NULL)
		{
			ret=obj->term->check_coord(x-obj->x0*charWidth,y-obj->y0*charHeight,1);

			if(ret)
			{
				obj->term->paint();
			}

			return(ret);
		}
	}
	coord_x=x;
	coord_y=y;
	for (int i=0;i<(int)objects->size();i++)
	{
		object *im=objects->Get(i);
		if(im->arg!=X_IMAGE || im->img==NULL || im->x1==0)
			continue;
		if(im->fill==true)
		{
			if(!(x>im->x0 && x<(im->x0+im->x1) && y>im->y0 && y<(im->y0+im->y1)))
			{
				im->fill=false;
				update[im->y0/charHeight+1]=true;
				ret=true;
			}
		}
		if(x>im->x0 && x<(im->x0+im->x1) && y>im->y0 && y<(im->y0+im->y1))
		{
			if(im->fill==false)
			{
				im->fill = true;
				update[im->y0/charHeight+1]=true;
				ret=true;
			}
		}
	}
	return(ret);
}

void   Terminal::set_active(bool act)
{
	active=act;
	for (int i=0;i<(int)objects->size();i++)
	{
		object *im = objects->Get(i);
		if(im->arg==X_SCREEN && im->term!=NULL)
			im->term->set_active(act);
	}
	if(act==true)
	{
		update[0]=true;
		paint();
	}
}

void  Terminal::set_current_screen(int num)
{
	if(parent!=NULL)
		parent->set_current_screen(num);
	else
	{
		for(int i=0;i<objects->size();i++)
		{
			object *obj=objects->Get(i);
			if(obj->arg==X_SCREEN && obj->color==num)
			{
				current_screen=num;
				last_screen=num;
				return;
			}
		}
		current_screen=0;
	}
}

void   Terminal::putString(CString s)
{
	putStringLen(s,s.GetLength());
}

void   Terminal::putStringLen(const char *s, int len)
{
	int  i;
	if(s==NULL || len==0)
		return;
	object *obj=NULL;
	for (i=0;i<len;i++)
	{
		obj = get_screen();

		if(obj!=NULL)
			obj->term->scr->putChar(s[i]);
		else
			scr->putChar(s[i]);
	}
	if(obj!=NULL)
	{
		obj->term->scr->setCursorPos();
		obj->term->paint();
	}
	scr->setCursorPos();
}
//---------------------------------------------------------------------------
