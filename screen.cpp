// screen.cpp: implementation of the screen class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "screen.h"
#include "Display.h"
#include "channel.h"
#include "terminalView.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

int __fastcall checkBounds(int value, int lower, int upper)
{
	if(value < lower)
		return lower;
	if(value > upper)
		return upper;
	return value;
}

void   bcopy(const void *a,void *b,size_t len)
{
        memcpy(b,a,len);
}

int    bcmp(const void *a, const void *b,size_t len)
{
        return(memcmp(a,b,len));
}

void   bzero(void *b, size_t len)
{
        memset(b,0,len);
}

static int conv_color(int col)
{
        switch(col)
        {
                case 1:
                        return 4;
                case 3:
                        return 13;
                case 4:
                        return 1;
                case 5:
                        return 5;
                case 6:
                        return 3;
                default:
                        return col;
        }
}

screen::screen(int h, int l, Terminal *s_io)
{
//	bzero(this,sizeof *this);
	mX=l;
        mY=h;
	client=(struct pics *)calloc((mX+1)*(mY+1),sizeof (struct pics));
	cW=s_io->charWidth;
	cH=s_io->charHeight;
	term=s_io;
    term_state = TSTATE_DATA;
	Name_Image.Empty();
	bzero(&S_color,sizeof S_color);
	S_color.fg=7;
    clean_vars();
    lastwaslf = 1;
    bzero(Tabs,sizeof Tabs);
    for (int i=0;i<sizeof Tabs/sizeof (int);i+=8)
		Tabs[i]=1;
    ch_count=0;
    light=0;
	C=0;
	R=0;
//        s_io->channel->term_name;
}

screen::~screen()
{
	free(client);
}


void   screen::copy(screen *tmp)
{
        if(tmp==NULL)
                return;
        for(int i=0;i<mY && i<tmp->mY;i++)
        {
                int len;
                if(tmp->mX>mX)
                        len=mX;
                else    len=tmp->mX;
                bcopy(tmp->client+i*tmp->mX,client+i*mX,len);
        }
}

int  __fastcall screen::Width(int x)
{
	if(x!=0)
		mX=x;
	return(mX);
}

int   screen::Height(int y)
{
	if(y!=0)
		mY=y;
	return(mY);
}

int   screen::charWidth(int w)
{
	if(w!=0)
		cW=w;
	return(term->charWidth);
}

int   screen::charHeight(int h)
{
	if(h!=0)
		cH=h;
	return(term->ClientHeight);
}

void   screen::_SetCursor(int row,int col)
{
        R = (row<0)?0:row;
        C = (col<0)?0:col;
        if (R>mY) R = mY;
}

void   screen::Set_Color(struct clr color)
{
	S_color=color;
}

void   screen::black_white(int x,int y,int l,int h,int atr)
{
        if(l==0 && h==0)
        {
                S_color.bwlt=atr>0;
                return;
        }
        for(int j=y;j<y+h && j<mY;j++)
        {
                for(int i=x;i<x+l && i<mX;i++)
                {
                        if(client[j*mX+i].clr.bwlt==atr>0)
                                continue;
                        if(atr>0)
                                client[j*mX+i].clr.bwlt=1;
                        else
                                client[j*mX+i].clr.bwlt=0;
                        term->markLine(j,1);
                }
        }
}

void   screen::Set_Font(unsigned char font)
{
	S_color.font=font;
}

void   screen::Set_Char(int x, int y, char ch)
{
	x = checkBounds(x, 0, mX-1);
	y = checkBounds(y, 0, mY-1);
        if(client[y*mX+x].ch==ch)
                return;
	client[y*mX+x].ch=ch;
        term->markLine(y,1);
}

void   screen::Color(int x, int y, struct clr color)
{
	x = checkBounds(x, 0, mX-1);
	y = checkBounds(y, 0, mY-1);
        if(!memcmp(&client[y*mX+x].clr,&color,sizeof color))
                return;
	client[y*mX+x].clr=color;
}

struct  clr  screen::currColor()
{
	return S_color;
}

int   screen::currX()
{
	return x_c;
}

int   screen::currY()
{
	return y_c;
}

struct  pics  __fastcall screen::Char(int x,int y)
{
	x = checkBounds(x, 0, mX-1);
	y = checkBounds(y, 0, mY-1);
	return(client[y*mX+x]);
}

CString screen::Chars(int y, int x, int l)
{
        static char str[256];
        y = checkBounds(y, 0, mY-1);
        x = checkBounds(x, 0, mX-1);
        l = checkBounds(l, 0, mX-x);
        bzero(str,sizeof str);
        for(int i=0;i<l;i++)
        {
                str[i]=Char(x+i,y).ch;
        }
        return(str);
}

unsigned char   screen::Value(int y, int x)
{
        return(Char(x,y).ch);
}

clr __fastcall screen::Attr(int y, int x)
{
        return(Char(x,y).clr);
}

struct clr   screen::Color(int x,int y)
{
	x = checkBounds(x, 0, mX-1);
	y = checkBounds(y, 0, mY-1);
	return(client[y*mX+x].clr);
}

void   screen::erase_screen()
{
	erase_screen(S_color);
}

void   screen::erase_screen(struct clr color)
{
	for(int y = 0; y < mY; y++)
	{
		for(int x = 0; x < mX; x++)
		{
			client[y*mX+x].ch=' ';
			client[y*mX+x].clr=color;
		}
                term->markLine(y,1);
	}
}

void   screen::set_bg_box(int x,int y,int l,int h,int color)
{
        for(int j=y;j<y+h && j<mY;j++)
        {
                for(int i=x;i<x+l && i<mX;i++)
                        client[j*mX+i].clr.bg=color;
                term->markLine(j,1);
        }
}

void   screen::erase_line()
{
	erase_line(S_color);
}

void   screen::erase_line(struct clr color)
{
	for(int x = x_c; x < mX; x++)
	{
		client[y_c*mX+x].ch=' ';
		client[y_c*mX+x].clr=color;
	}
        term->markLine(y_c,1);
}

void   screen::erase_area(int x,int y,int l,int h)
{
	erase_area(x,y,l,h,S_color);
}

void   screen::Erase_Area(int x,int y,int l,int h)
{
	struct clr color;
	bzero(&color,sizeof color);
	erase_area(x,y,l,h,color);
}

void   screen::erase_area(int x,int y,int l,int h,struct clr color)
{
	for(int j=y;j<y+h && j<mY;j++)
	{
		for(int i=x;i<x+l && i<mX;i++)
		{
			client[j*mX+i].ch=' ';
			client[j*mX+i].clr=color;
		}
                term->markLine(j,1);
	}
}

void   screen::putChar(int x, int y, char ch)
{
	putChar(x,y,ch,S_color);
}

void   screen::putChar(int x, int y, char ch, struct clr color)
{
	x = checkBounds(x, 0, mX - 1);
	y = checkBounds(y, 0, mY - 1);
        if(client[y*mX+x].ch==ch && !memcmp(&client[y*mX+x].clr,&color,sizeof color))
                return;
        term->markLine(y,1);
	client[y*mX+x].ch  = ch;
	client[y*mX+x].clr = color;
}

void   screen::put_a1(int x,int y,int atr1)
{
	x = checkBounds(x, 0, mX - 1);
	y = checkBounds(y, 0, mY - 1);
        if(client[y*mX+x].clr.atr1==atr1)
                return;
        term->markLine(y,1);
	client[y*mX+x].ch=atr1&03;
}

void   screen::put_a2(int x,int y,int atr2)
{
	x = checkBounds(x, 0, mX - 1);
	y = checkBounds(y, 0, mY - 1);
        if(client[y*mX+x].clr.atr2==atr2)
                return;
        term->markLine(y,1);
	client[y*mX+x].clr.atr2=atr2&03;
}

void   screen::put_frame(int x,int y,int frame)
{
	x = checkBounds(x, 0, mX - 1);
	y = checkBounds(y, 0, mY - 1);
        if(client[y*mX+x].clr.frame==frame)
                return;
        term->markLine(y,1);
	client[y*mX+x].clr.frame=frame;
}

void   screen::put_font(int x,int y,int font)
{
	x = checkBounds(x, 0, mX - 1);
	y = checkBounds(y, 0, mY - 1);
        if(client[y*mX+x].clr.font==font)
                return;
        term->markLine(y,1);
	client[y*mX+x].clr.font=font;
}

void   screen::insertChar(int x, int y, char ch)
{
	insertChar(x,y,ch,S_color);
}

void   screen::insertChar(int x, int y, char ch, struct clr color)
{
	x = checkBounds(x, 0, mX - 1);
	y = checkBounds(y, 0, mY - 1);
	bcopy(client+y*mX+x,client+y*mX+x+1,(mX - x - 1)*sizeof (struct pics));
	putChar(x, y, ch, color);
}

void   screen::deleteChar(int x, int y)
{
	x = checkBounds(x, 0, mX - 1);
	y = checkBounds(y, 0, mY - 1);
	if(x < mX - 1)
		bcopy(client+y*mX+x+1,client+y*mX+x,(mX - x - 1)*sizeof (struct pics));
	putChar(mX - 1, y, ' ');
}

void   screen::insertLine(int y)
{
	insertLine(y, 1);
}

void   screen::insertLine(int y, int n)
{
        insertLine(0,y,mX,mY,n);
}

void   screen::insertLine(int x, int y, int w, int h, int n)
{
	y = checkBounds(y, 0, mY-1);
	x = checkBounds(x, 0, mX-1);
	h = checkBounds(h, 0, mY-y);
	w = checkBounds(w, 0, mX-x);
	n = checkBounds(n, 0, h-1);
	for(int i=y+h-n-1;i>=y;i--)
		bcopy(client+(i)*mX+x,client+(i+1)*mX+x,w*sizeof (struct pics));
	for(int j=0;j<n;j++)
	{
		for(int i=x;i<x+w;i++)
		{
			client[(y+j)*mX+i].ch=' ';
			bzero(&client[(y+j)*mX+i].clr,sizeof (struct clr));
		}
	}
        term->markLine(y,h);
}

void   screen::deleteLine(int x, int y, int w, int h, int n)
{
	y = checkBounds(y, 0, mY-1);
	x = checkBounds(x, 0, mX-1);
	h = checkBounds(h, 0, mY-y);
	w = checkBounds(w, 0, mX-x);
	n = checkBounds(n, 0, h-1);
	for(int i=y;i<y+h-n;i++)
		bcopy(client+(i+n)*mX+x,client+(i)*mX+x,w*sizeof (struct pics));
	for(int j=0;j<n;j++)
	{
		for(int i=x;i<x+w;i++)
		{
			client[(y+h-j-1)*mX+i].ch=' ';
			bzero(&client[(y+h-j-1)*mX+i].clr,sizeof (struct clr));
		}
	}
        term->markLine(y,mY-y);
}

void    screen::deleteLine(int l)
{
	deleteLine(l,1);
}

void   screen::deleteLine(int l, int n)
{
        deleteLine(0,l,mX,mY,n);
}

void   screen::deleteArea(int c, int l, int w, int h)
{
	c = checkBounds(c, 0, mX - 1);
	l = checkBounds(l, 0, mY - 1);

	erase_area(c,l,w,h);
}

void   screen::pos(int x, int y)
{
	x_c=checkBounds(x,0,mX-1);
	y_c=checkBounds(y,0,mY-1);
}

void   screen::putString(CString buf)
{
        int len=buf.GetLength();
        if(len==0)
                return;
		char *ch=buf.GetBuffer(len);
		for(int i=0;i<len;i++)
		{
			if(ch[i]==0)
				continue;
			putChar(ch[i]);
        }
}

void   screen::putString(char *str,int len)
{
	if(str==NULL)
		return;
	if(len==0)
		len=strlen(str);
	for(int i=0;i<len;i++)
		putChar(str[i]);
}

void   screen::clean_vars()
{
        DCEvar    = 0;
        for(int i=0;i<10;i++)
                DCEvars[i]=0;
        Name_Image.Empty();
}
void   screen::putChar(unsigned char c)
{
        int  rows = mY;
        int  columns = mX;
        int  tm = 0;
        int  bm = mY-1;
		int i;

//        term->markLine(R,1);

        if(ch_count==sizeof buf-1)
                ch_count=0;
        buf[ch_count++]=c;
        buf[ch_count]=0;

        switch (term_state)
        {
			case TSTATE_DATA:
				ch_count=0;
			switch (c)
			{
			case ESC:
				term_state = TSTATE_ESC;
				clean_vars();
				lastwaslf=0;
				ch_count=1;
				break;
			case '\b':
				C--;
				if (C<0)
					C=0;
				lastwaslf = 0;
				break;
			case '\t':
				if (insertmode == 1)
				{
					int  newc;

					newc = C;
					do
					{
						insertChar(C,R,' ',S_color);
						newc++;
					}
					while (newc<columns && Tabs[newc]==0);
				}
				else
				{
					do
					{
						putChar(C++,R,' ',S_color);
					}
					while (C<columns && (Tabs[C]==0));
				}
				lastwaslf = 0;
				break;
			case '\r':
				C=0;
				break;
			case '\n':
				if (lastwaslf!=0 && lastwaslf!=c)   //  Ray: I do not understand this logic.
					break;
				lastwaslf=c;
				/******C = 0;*/
				if (R == tm - 1 || R == bm || R >= rows - 1)
					deleteLine(0);
				else
					R++;
				break;
			default:
				lastwaslf=0;
				if (c<' ')
					break;
				if(C >= columns)
				{
					if(R < rows - 1)
						R++;
					else
						deleteLine(0);
					C = 0;
				}
				if (insertmode==1)
					insertChar(C, R, c, S_color);
				else
					putChar(C, R, c, S_color);
				C++;
				break;
			}
			break;
		case TSTATE_ESC:
			switch (c)
			{
			case '#':
				term_state = TSTATE_ESCSQUARE;
				break;
			case '[':
				term_state = TSTATE_CSI;
				break;
			case '?':
			case '|':
				term_state=TSTATE_ESCO;
				break;
			case 'M': // IL
				if ((R>=tm) && (R<=bm)) // in scrolregion
					insertLine(R,1);
				/****** else do nothing ; */
				term_state = TSTATE_DATA;
				break;
			case 'H':
				/****** right border probably ...*/
				if (C>=columns)
					C=columns-1;
				Tabs[C] = 1;
				term_state = TSTATE_DATA;
				break;
			case '0':
				term->No_Cursor(true);
				term_state = TSTATE_DATA;
				break;
			case '1':
				term->No_Cursor(false);
				term_state = TSTATE_DATA;
				break;
			default:
				term_state = TSTATE_DATA;
				break;
			}
			break;
		case TSTATE_START:
			if(c=='"')
			{
				term_state = TSTATE_ESCO;
				break;
			}
            Name_Image+=c;
			break;
		case TSTATE_ESCO:
			switch(c)
			{
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				DCEvars[DCEvar]=DCEvars[DCEvar]*10+((int)c)-48;
				break;
			case ';':
                                if(DCEvar==(sizeof DCEvars)/(sizeof DCEvars[0])-1)
                                        break;
				DCEvar++;
				DCEvars[DCEvar] = 0;
				break;
			case '"':
				term_state = TSTATE_START;
				break;
			case 'v':
				term->channels->main_form->Visual_Eff_Exec(DCEvars[0],DCEvars[1],DCEvars[2],DCEvars[3],DCEvars[4],DCEvars[5]);
				term_state = TSTATE_DATA;
				break;
			case 'h':
			case 'l':

				if(DCEvars[0]==25)
					term->No_Cursor(c=='l');
				term_state = TSTATE_DATA;
				break;

			case '!':
				DCEvars[3]=DCEvars[0]*term->charWidth;
				DCEvars[4]=DCEvars[1]*term->charHeight;
				DCEvars[0]=X_IMAGE;
				DCEvars[1]=C*term->charWidth;
				DCEvars[2]=R*term->charHeight;
			case '*': // put object
				if(DCEvars[0]==X_IMAGE)
					term->Load_Image(Name_Image,DCEvars[1],DCEvars[2],DCEvars[3],DCEvars[4],DCEvars[5]);
				else
					term->putObject(DCEvars[0],DCEvars[1],DCEvars[2],DCEvars[3],DCEvars[4],DCEvars[5],DCEvars[6],DCEvars[7],DCEvars[8],NULL,Name_Image);

				term_state = TSTATE_DATA;
				break;
			case '`': // put object in terminal coordinates
				term->putObject(DCEvars[0],DCEvars[1]*term->charWidth,DCEvars[2]*term->charHeight,DCEvars[3]*term->charWidth,DCEvars[4]*term->charHeight,DCEvars[5],DCEvars[6],DCEvars[7],DCEvars[8],NULL,Name_Image);
				term_state = TSTATE_DATA;
				break;
			case 'P':
				term->putVect(DCEvars[0],DCEvars[1],X_POLYGON);
				term_state = TSTATE_DATA;
				break;
			case 'L':
				term->putVect(DCEvars[0],DCEvars[1],X_POLYLINE);
				term_state = TSTATE_DATA;
				break;
			case '$':
				if(DCEvar<2)  // delete object
					term->Delete_All_Objects(DCEvars[0]);
				else
					term->Delete_Object(DCEvars[0],DCEvars[1],DCEvars[2]);
				term_state = TSTATE_DATA;
				break;
			case '%':
				term->Show_Image(Name_Image);
				term_state = TSTATE_DATA;
				break;
			case '&':
				term->Show_Document(Name_Image);
				term_state = TSTATE_DATA;
				break;
			case '>':
				term->putScreen(DCEvars[0],DCEvars[1],DCEvars[2],DCEvars[3],DCEvars[4],DCEvars[5],DCEvars[6]);
				term_state = TSTATE_DATA;
				break;
			case '<':
				term->set_current_screen(DCEvars[0]);
				term_state = TSTATE_DATA;
				break;
			case 'f':
				S_color.font = DCEvars[0]<<2;   // font
				S_color.font +=DCEvars[1]%4;    // normal, italic, bold, italic+bold
				term_state = TSTATE_DATA;
				break;
			}
			break;
// old version.

		case TSTATE_ESCSQUARE:
			switch (c)
			{
			case'|':
/*
				if(Name_Image.length()==0)
					term->del_menu(C,R);
				else
					term->add_menu(C,R,Name_Image);
*/
				Name_Image.Empty();
				term_state = TSTATE_DATA;
				break;
			case '!':
				if(Name_Image.GetLength()==0)
				{
					term->Delete_All_Objects(X_IMAGE);
				}
				else
				{
					term->Get_Image(Name_Image,C,R);
				}
				term_state = TSTATE_DATA;
				Name_Image.Empty();
				break;
			case '%':
                term->Show_Image(Name_Image);
                term_state = TSTATE_DATA;
                Name_Image.Empty();
				term_state = TSTATE_DATA;
				break;
			case '&':
				term->Show_Document(Name_Image);
				term_state = TSTATE_DATA;
				Name_Image.Empty();
				break;
			case '@':
				term_state = TSTATE_DATA;
                if(Name_Image.GetLength()>0)
                {
					term->channels->Title=Name_Image;
//                  Application->Title=Name_Image;
                }
				Name_Image.Empty();
			default:
				Name_Image+=c;
				break;
			}
			break;
		case TSTATE_CSI:
			switch (c)
			{
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				DCEvars[DCEvar]=DCEvars[DCEvar]*10+((int)c)-48;
				break;
			case ';':
				DCEvar++;
				DCEvars[DCEvar] = 0;
				break;
			case 'g':
				/****** used for tabsets */
				switch (DCEvars[0])
				{
				case 3:/* clear them */
                    bzero(Tabs,sizeof Tabs);
					break;
				case 0:
					Tabs[C] = 0;
					break;
				}
				term_state = TSTATE_DATA;
				break;
			case 'h':
				switch (DCEvars[0])
				{
				case 4:
					insertmode = 1;
					break;
				default:
					break;
				}
				term_state = TSTATE_DATA;
				break;
			case 'l':
				switch (DCEvars[0])
				{
				case 4:
					insertmode = 0;
					break;
				}
				term_state = TSTATE_DATA;
				break;

			case 'A': // CUU
				{
					int limit;
					if (R > bm)
						limit = bm+1;
					else if (R >= tm)
						limit = tm;
					else
						limit = 0;
					if (DCEvars[0]==0)
						R--;
					else
						R-=DCEvars[0];
					if (R < limit)
						R = limit;
					term_state = TSTATE_DATA;
					break;
				}

			case 'B':	// CUD
				{ // cursor down n (1) times
					int limit;
					if (R < tm)
						limit = tm-1;
					else if (R <= bm)
					{
						limit = bm;
					}
					else
						limit = rows - 1;
					if (DCEvars[0]==0)
						R++;
					else
						R+=DCEvars[0];
					if (R > limit)
						R = limit;
					else
					{
					}
					term_state = TSTATE_DATA;
					break;
				}
			case 'C':
				if (DCEvars[0]==0)
					C++;
				else
					C+=DCEvars[0];
				if (C>columns-1)
					C=columns-1;
				term_state = TSTATE_DATA;
				break;
			case 'd': // CVA
				R = DCEvars[0];
				term_state = TSTATE_DATA;
				break;
			case 'D':
				if (DCEvars[0]==0)
					C--;
				else
					C-=DCEvars[0];
				if (C<0) C=0;
				term_state = TSTATE_DATA;
				break;
			case 'E':       // erase rectangle area
				erase_area(DCEvars[0],DCEvars[1],DCEvars[2],DCEvars[3],S_color);
				term_state = TSTATE_DATA;
				break;
			case 'G':  /* CUP  / cursor absolute column */
				C = DCEvars[0];
				term_state = TSTATE_DATA;
				break;
			case 'H':  /* CUP  / cursor position */
				/****** gets 2 arguments */
				if(DCEvar==0)
					_SetCursor(0,0);
				else
					_SetCursor(DCEvars[0]-1,DCEvars[1]-1);
				term_state = TSTATE_DATA;
				break;
			case 'F':  /* move cursor 2 */
				/****** gets 2 arguments */
				R = DCEvars[0]-1;
				C = DCEvars[1]-1;
				if (C<0) C=0;
				if (R<0) R=0;
				term_state = TSTATE_DATA;
				break;
			case 'L':
				if (DCEvar>3) // insert line in area
					insertLine(DCEvars[0],DCEvars[1],DCEvars[2],DCEvars[3],DCEvars[4]);
				else if (DCEvars[0]==0)
					insertLine(R);
				else   // insert n lines
					insertLine(R,DCEvars[0]);
				term_state = TSTATE_DATA;
				break;
			case 'M':
				if (DCEvar>3) // delete line in area
					deleteLine(DCEvars[0],DCEvars[1],DCEvars[2],DCEvars[3],DCEvars[4]);
				else if (DCEvars[0]==0)
					deleteLine(R);
				else
					deleteLine(R,DCEvars[0]);
				term_state = TSTATE_DATA;
				break;
			case 'K':
				switch (DCEvars[0])
				{
				case 0:/*clear to right*/
					if (C<columns-1)
						deleteArea(C,R,columns-C,1);
					break;
				case 1:/*clear to the left*/
					if (C>0)
						deleteArea(0,R,C,1);    // Ray: Should at least include character before this one, not C-1
					break;
				case 2:/*clear whole line */
					deleteArea(0,R,columns,1);
					break;
				}
				term_state = TSTATE_DATA;
				break;
			case 'J':
				/****** clear display.below current line */
				switch (DCEvars[0])
				{
				case 0:
					if (R<rows-1)
						deleteArea(0,R + 1,columns,rows-R-1);
					if (C<columns-1)
						deleteArea(C,R,columns-C,1);
					break;
				case 1:
					if (R>0)
						deleteArea(0,0,columns,R-1);
					if (C>0)
						deleteArea(0,R,C,1);
					break;
				case 2:
					deleteArea(0,0,columns,rows);
					break;
				}
				term_state = TSTATE_DATA;
				break;
			case '@':
				for (i=0;i<=DCEvars[0];i++)
					insertChar(C,R,' ',S_color);
				term_state = TSTATE_DATA;
				break;
			case 'P':
				for (i=0;i<=DCEvars[0];i++)
					deleteChar(C,R);
				term_state = TSTATE_DATA;
				break;
			case 'Z':
               {
					char tmp[256];
					sprintf(tmp,"%d;%d;%d;%dH",term->charHeight,term->charWidth,mX,mY);
                    CString str=tmp;
                    term->Write(str);
					term_state = TSTATE_DATA;
					break;
                }
			case 'X':
				black_white(DCEvars[0],DCEvars[1],DCEvars[2],DCEvars[3],1);
				term_state = TSTATE_DATA;
				break;
			case 'Y':
                if(DCEvar<3)
					black_white(0,0,0,0,DCEvars[0]);
                else
        			black_white(DCEvars[0],DCEvars[1],DCEvars[2],DCEvars[3],DCEvars[4]);
				term_state = TSTATE_DATA;
				break;
			case 't':
				term->putObject(X_RECT,DCEvars[0],DCEvars[1],DCEvars[2],DCEvars[3],DCEvars[4],1,0,0,NULL,"");
				term_state = TSTATE_DATA;
				break;
			case 'x':
				term->putObject(X_LINE,DCEvars[0],DCEvars[1],DCEvars[2],DCEvars[3],DCEvars[4],1,0,0,NULL,"");
				term_state = TSTATE_DATA;
				break;
			case 'y':
				term->putVect(DCEvars[0],DCEvars[1],X_POLYLINE);
				term_state = TSTATE_DATA;
				break;
			case 'v':
				term->Delete_Last_Object(DCEvars[0]==0?X_LINE:DCEvars[0]);
				term_state = TSTATE_DATA;
			case 'z':
				if(DCEvars[2]==0 && DCEvars[3]==0)
					term->Delete_All_Objects(X_ARC);
				else
				{
					coord_x0=DCEvars[0]*term->charWidth ;
					coord_y0=DCEvars[1]*term->charHeight;
					int l=DCEvars[2]*term->charWidth;
				 	int h=DCEvars[3]*term->charHeight;
					if(l>h)
						h=l;
					else
						l=h;
					coord_x1=l;
					coord_y1=h;
					count=0;
				}
				term_state = TSTATE_DATA;
				break;
			case 'w':
				term->putObject(X_ARC,coord_x0,coord_y0,coord_x1,coord_y1,DCEvars[1],1,count,DCEvars[0],NULL,"");
				count+=DCEvars[0];
				term_state = TSTATE_DATA;
				break;
			case 's':
		      //		term->flush_screen();
				term_state = TSTATE_DATA;
				break;
			case 'o':
		      //		term->repaint();
				term_state = TSTATE_DATA;
				break;
			case '!':
				term->Delete_Object(X_IMAGE,DCEvars[0]*term->charWidth,DCEvars[1]*term->charHeight);
				term_state = TSTATE_DATA;
				break;
			case 'I':
				if(DCEvar==0)
;//                			term->init_color();
				else
					term->set_color(DCEvars[0],DCEvars[1],DCEvars[2],DCEvars[3]);
				term_state = TSTATE_DATA;
				break;
			case 'c':
				set_bg_box(DCEvars[0],DCEvars[1],DCEvars[2],DCEvars[3],DCEvars[4]);
				term_state = TSTATE_DATA;
				break;
			case 'a':
				{
					S_color.frame=DCEvars[0];
					term_state = TSTATE_DATA;
					break;
				}
			case 'f':
				{
					S_color.fg=DCEvars[0]&0xff;
					term_state = TSTATE_DATA;
					break;
				}
			case 'b':
				{
					S_color.bg=DCEvars[0]&0xff;
					term_state = TSTATE_DATA;
					break;
				}
			case 'm':  /* S_color as color, font */
/*
                                {
                                        int color=0;
                                        char *atr;
                                        atr=term->channel->Output_Code(22);
                                        if(atr!=NULL && sscanf(buf,atr,&color)==1)
                                        {
                                                S_color.fg=conv_color(color);
        					term_state = TSTATE_DATA;
                                                break;
                                        }
                                        atr=term->channel->Output_Code(23);
                                        if(atr!=NULL && sscanf(buf,atr,&color)==1)
                                        {
                                                S_color.bg=conv_color(color);
        					term_state = TSTATE_DATA;
                                                break;
                                        }
                                        atr=term->channel->Output_Code(24);
                                        if(atr!=NULL && !strcmp(buf,atr))
                                        {
                                                S_color.fg=7;
                                                S_color.bg=0;
        					term_state = TSTATE_DATA;
                                                break;
                                        }
                                        atr=term->channel->Output_Code(25);
                                        if(atr!=NULL && !strcmp(buf,atr))
                                        {
                                                S_color.bg=0;
                                                S_color.fg=7;
        					term_state = TSTATE_DATA;
                                                break;
                                        }
                                }
*/
				if (DCEvar == 0 && DCEvars[0]==0)
				{
					int font=S_color.font;
                    bzero(&S_color,sizeof S_color);
                    S_color.font=font;
				}
				else if(DCEvars[0]==2 && !strcmp(term->channels->term_type,"ANSI"))
				{
                    bzero(&S_color,sizeof S_color);
					S_color.fg=DCEvars[1];
					S_color.bg=DCEvars[2];
				}
				else for (int i=0;i<=DCEvar;i++)
				{
					int j=DCEvars[i];
                    if(!strcmp(term->channels->term_type,"UnixSpace"))
                    {
        				if(j>=11 && j<19)
	        			{
		        			S_color.font=(j-11)<<2;
			        	}
				        else
						{
					        switch (j)
						    {
        						case 0:
	        						if (DCEvar>0)
                                    {
										int font=S_color.font;
                                        bzero(&S_color,sizeof S_color);
                                        S_color.font=font;
                                    }
	        						break;
		        				case 1:
			        				erase_area(0,0,mX,mY,S_color);
				        			break;
        						case 2:
	        						S_color.x1= 1;
		        					break;
			        			case 3:
                                    S_color.x1= 2;
							        break;
        						case 4:
	        						S_color.font&=~0x3;
		        					break;
			        			case 5:
				        			S_color.font&=~0x3;
					        		S_color.font=0x1;
						        	break;
        						case 6:
	        						S_color.x1=0;
		        					break;
			        			case 7:
				        			S_color.fg=7;
					        		S_color.bg=0;
        							break;
								case 8:
                                    S_color.font&=~0x3;
                                    S_color.font=0x2;
                                    break;
        						case 9:
	        						S_color.fg=0;
		        					S_color.bg=0x7;
			        				break;
                                case 10:
                                    S_color.font&=~0x3;
                                    S_color.font=0x3;
                                    break;
							}
						}
					}
                    else
                    {
						if(j==1)
							light=1;
                        else if(j==21)
							light=0;
        				else if(j>=30 && j<=38)  // color fg
	        			{
/*
							if(!strcmp(term->channels->term_type,"FreeBSD"))
								S_color.fg=j-30;
							else
*/
								S_color.fg = conv_color(j-30);
                            if(light)
                            {
								if(S_color.fg==13)
									S_color.fg=14;
                                else
									S_color.fg+=8;
							}
			        	}
				        else if(j>=40 && j<48)  // color bg
					    {
/*
							if(!strcmp(term->channels->term_type,"FreeBSD"))
								S_color.bg=j-40;
							else
*/
								S_color.bg = conv_color(j-40);
	        			}
					}
				}
				term_state = TSTATE_DATA;
				break;
			default:
				term_state = TSTATE_DATA;
				break;
			}
			break;
		default:
			term_state = TSTATE_DATA;
			break;
        }
        if (C > columns)
                C = columns;
        if (R > rows)
                R = rows;
        if (C < 0)  C = 0;
        if (R < 0)  R = 0;
        term->SetCursorPos(C,R);
//        term->markLine(R,1);
}

void  screen::setCursorPos()
{
        term->SetCursorPos(C,R);
}
