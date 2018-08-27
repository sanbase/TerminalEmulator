//---------------------------------------------------------------------------
#include "stdafx.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;

#define new DEBUG_NEW
#endif

#include "Display.h"
#include "TBitmap.h"

#define SF_BITMAP   0
#define SF_FILLRECT 1
#define SF_BITMAP1  2
#define SF_CIRCLE   3
#define SF_TRIANGLE 4 

#define SF_WIDTH    1
#define SF_HEIGHT   2
#define	SF_TYPE     3
#define	SF_DATA     4

//#define pBitmap main_form
//static TImage *icons[32];
void Terminal::draw_box(COLOR clr,unsigned int c,int x,int y,int cw, int ch,int atr)
{
	static int fontdata[][12]=
	{
		{
			176,8,8,SF_BITMAP,/**** LIGHT SHADE */
			0x22, /**** 00100010 */
			0x88, /**** 10001000 */
			0x22, /**** 00100010 */
			0x88, /**** 10001000 */
			0x22, /**** 00100010 */
			0x88, /**** 10001000 */
			0x22, /**** 00100010 */
			0x88, /**** 10001000 */
		}
		,
		{
			177,8,8,SF_BITMAP,/**** MEDIUM SHADE  0x2592*/
			0x55, /**** 01010101 */
			0xaa, /**** 10101010 */
			0x55, /**** 01010101 */
			0xaa, /**** 10101010 */
			0x55, /**** 01010101 */
			0xaa, /**** 10101010 */
			0x55, /**** 01010101 */
			0xaa, /**** 10101010 */
		}
		,
		{
			178,8,8,SF_BITMAP,/**** DARK SHADE */
			0x77, /**** 01110111 */
			0xdd, /**** 11011101 */
			0x77, /**** 01110111 */
			0xdd, /**** 11011101 */
			0x77, /**** 01110111 */
			0xdd, /**** 11011101 */
			0x77, /**** 01110111 */
			0xdd, /**** 11011101 */
		}
		,
		{
			179,8,8,SF_FILLRECT,/****BOX DRAWINGS LIGHT VERTICAL*/
			0x3018,
		}
		,
		{
			180,8,8,SF_FILLRECT,/**** BOX DRAWINGS LIGHT VERTICAL AND LEFT */
			0x3018,
			0x0431,
		}
		,
		{
			181,8,8,SF_FILLRECT,/****BOX DRAWINGS VERTICAL SINGLE AND LEFT DOUBLE*/
			0x3018,
			0x0331,
			0x0531,
		}
		,
		{
			182,8,8,SF_FILLRECT,/**** BOX DRAWINGS VERTICAL DOUBLE AND LEFT SINGLE */
			0x2018,
			0x5018,
			0x0421,
		}
		,
		{
			183,8,8,SF_FILLRECT,/**** BOX DRAWINGS DOWN DOUBLE AND LEFT SINGLE */
			0x0481,
			0x2414,
			0x5414,
		}
		,
		{
			184,8,8,SF_FILLRECT,/**** BOX DRAWINGS DOWN SINGLE AND LEFT DOUBLE */
			0x3316,
			0x0331,
			0x0531,
		}
		,
		{
			185,8,8,SF_FILLRECT,/**** BOX DRAWINGS DOUBLE VERTICAL AND LEFT*/
			0x2013,
			0x0331,
			0x0531,
			0x2513,
			0x5018,
		}
		,
		{
			186,8,8,SF_FILLRECT,/**** BOX DRAWINGS DOUBLE VERTICAL */
			0x2018,
			0x5018,
		}
		,
		{
			187,8,8,SF_FILLRECT,/****  BOX DRAWINGS DOUBLE DOWN AND LEFT */
			0x0361,
			0x5315,
			0x0531,
			0x2513,
		}
		,
		{
			188,8,8,SF_FILLRECT,/**** BOX DRAWINGS DOUBLE UP AND LEFT */
			0x2013,
			0x0331,
			0x5015,
			0x0561,
		}
		,
		{
			189,8,8,SF_FILLRECT,/**** BOX DRAWINGS UP DOUBLE AND LEFT SINGLE */
			0x2014,
			0x5014,
			0x0471,
		}
		,
		{
			190,8,8,SF_FILLRECT,/**** BOX DRAWINGS UP SINGLE AND LEFT DOUBLE */
			0x3015,
			0x0331,
			0x0541,
		}
		,
		{
			191,8,8,SF_FILLRECT,/**** BOX DRAWINGS LIGHT DOWN AND LEFT */
			0x0441,
			0x3513,
		}
		,
		{
			192,8,8,SF_FILLRECT,/**** BOX DRAWINGS LIGHT UP AND RIGHT */
			0x3015,
			0x4441,
		}
		,
		{
			193,8,8,SF_FILLRECT,/**** BOX DRAWINGS LIGHT UP AND HORIZONTAL */
			0x3014,
			0x0481,
		}
		,
		{
			194,8,8,SF_FILLRECT,/**** BOX DRAWINGS LIGHT DOWN AND HORIZONTAL */
			0x0481,
			0x3513,
		}
		,
		{
			195,8,8,SF_FILLRECT,/**** BOX DRAWINGS LIGHT VERTICAL AND RIGHT */
			0x3018,
			0x4461,
		}
		,
		{
			196,8,8,SF_FILLRECT,/**** BOX DRAWINGS LIGHT HORIZONTAL */
			0x0481,
		}
		,
		{
			197,8,8,SF_FILLRECT,/**** BOX DRAWINGS LIGHT VERTICAL AND HORIZONTAL */
			0x3018,
			0x0481,
		}
		,
		{
			198,8,8,SF_FILLRECT,/**** BOX DRAWINGS VERTICAL SINGLE AND RIGHT DOUBLE */
			0x3018,
			0x4341,
			0x4541,
		}
		,
		{
			199,8,8,SF_FILLRECT,/**** BOX DRAWINGS VERTICAL DOUBLE AND RIGHT SINGLE */
			0x2018,
			0x5018,
			0x5431,
		}
		,
		{
			200,8,8,SF_FILLRECT,/**** BOX DRAWINGS DOUBLE UP AND RIGHT */
			0x2016,
			0x5014,
			0x6321,
			0x3551,
		}
		,
		{
			201,8,8,SF_FILLRECT,/**** BOX DRAWINGS DOUBLE DOWN AND RIGHT */
			0x2361,
			0x2414,
			0x5513,
			0x6521,
		}
		,
		{
			202,8,8,SF_FILLRECT,/**** BOX DRAWINGS DOUBLE UP AND HORIZONTAL */
			0x2013,
			0x0331,
			0x5013,
			0x5331,
			0x0581,
		}
		,
		{
			203,8,8,SF_FILLRECT,/**** BOX DRAWINGS DOUBLE DOWN AND HORIZONTAL */
			0x0381,
			0x0531,
			0x2513,
			0x5513,
			0x5531,
		}
		,
		{
			204,8,8,SF_FILLRECT,/**** BOX DRAWINGS DOUBLE VERTICAL AND RIGHT */
			0x2018,
			0x5013,
			0x5331,
			0x5531,
			0x5612,
		}
		,
		{
			205,8,8,SF_FILLRECT,/**** BOX DRAWINGS DOUBLE HORIZONTAL */
			0x0381,
			0x0581,
		}
		,
		{
			206,8,8,SF_FILLRECT,/**** BOX DRAWINGS DOUBLE VERTICAL AND HORIZONTAL */
			0x2013,
			0x0331,
			0x5013,
			0x5331,
			0x0531,
			0x2513,
			0x5531,
			0x5612,
		}
		,
		{
			207,8,8,SF_FILLRECT,/**** BOX DRAWINGS UP SINGLE AND HORIZONTAL DOUBLE */
			0x3013,
			0x0381,
			0x0581,
		}
		,
		{
			208,8,8,SF_FILLRECT,/**** BOX DRAWINGS UP DOUBLE AND HORIZONTAL SINGLE */
			0x2014,
			0x5014,
			0x0481,
		}
		,
		{
			209,8,8,SF_FILLRECT,/**** BOX DRAWINGS DOWN SINGLE AND HORIZONTAL DOUBLE */
			0x0381,
			0x0581,
			0x3513,
		}
		,
		{
			210,8,8,SF_FILLRECT,/**** BOX DRAWINGS DOWN DOUBLE AND HORIZONTAL SINGLE */
			0x0481,
			0x2513,
			0x5513,
		}
		,
		{
			211,8,8,SF_FILLRECT,/**** BOX DRAWINGS UP DOUBLE AND RIGHT SINGLE */
			0x2014,
			0x5014,
			0x2461,
		}
		,
		{
			212,8,8,SF_FILLRECT,/**** BOX DRAWINGS UP SINGLE AND RIGHT DOUBLE */
			0x3016,
			0x4341,
			0x4551,
		}
		,
		{
			213,8,8,SF_FILLRECT,/**** BOX DRAWINGS DOWN SINGLE AND RIGHT DOUBLE */
			0x3316,
			0x4341,
			0x4541,
		}
		,
		{
			214,8,8,SF_FILLRECT,/**** BOX DRAWINGS DOWN DOUBLE AND RIGHT SINGLE */
			0x2461,
			0x2513,
			0x5513,
		}
		,
		{
			215,8,8,SF_FILLRECT,/**** BOX DRAWINGS VERTICAL DOUBLE AND HORIZONTAL SINGLE */
			0x2018,
			0x5018,
			0x0481,
		}
		,
		{
			216,8,8,SF_FILLRECT,/**** BOX DRAWINGS VERTICAL SINGLE AND HORIZONTAL DOUBLE */
			0x3018,
			0x0381,
			0x0581,
		}
		,
		{
			217,8,8,SF_FILLRECT,/**** BOX DRAWINGS LIGHT UP AND LEFT */
			0x3015,
			0x0431,
		}
		,
		{
			218,8,8,SF_FILLRECT,/**** BOX DRAWINGS LIGHT DOWN AND RIGHT */
			0x3451,
			0x3513,
		}
		,
		{
			219,8,8,SF_FILLRECT,/**** FULL BLOCK */
			0x0088,
		}
		,
		{
			220,8,8,SF_FILLRECT,/**** LOWER HALF BLOCK */
			0x0484,
		}
		,
		{
			221,8,8,SF_FILLRECT,/**** LEFT HALF BLOCK */
			0x0048,
		}
		,
		{
			222,8,8,SF_FILLRECT,/**** RIGHT HALF BLOCK */
			0x4048,
		}
		,
		{
			223,8,8,SF_FILLRECT,/**** UPPER HALF BLOCK */
			0x0084,
		}
		,
		{
			248,8,8,SF_CIRCLE,
			0x4442,
		}
		,
		{
			301,8,8,SF_FILLRECT,
			0x3015,
		}
		,
		{
			302,8,8,SF_FILLRECT,
			0x4441,
		}
		,
		{
			303,8,8,SF_FILLRECT,
			0x3513,
		}
		,
		{
			304,8,8,SF_FILLRECT,
			0x0441,
		}

	};
	if(c<' ')
	{
		/***
		if(icons[c]==NULL)
						{
							char name[64];
							sprintf(name,"Images/CX/icon%02d.gif",c);
							Loader *l=new Loader(this,name,&icons[c]);
							l->Resume();
						}
						if(icons[c]!=NULL)
							pBitmap->Canvas->StretchDraw(TRect(x,y,x+charWidth,y+charHeight),icons[c]->Picture->Graphic);
				*/
		return;
	}


	int i,j,entry,w,h;
	double	dw,dh;
	for(entry=0;entry<sizeof fontdata/sizeof fontdata[0];entry++)
	{
		if(fontdata[entry][0]==(int)c)
			break;
	}
	if(entry==sizeof fontdata/sizeof fontdata[0])
		return;
	int fontwidth  = fontdata[entry][SF_WIDTH];
	int fontheight = fontdata[entry][SF_HEIGHT];
	dw = cw*1.0/fontwidth;
	dh = ch*1.0/fontheight;
	
	COLOR fg,bg;
	if(atr==2)
	{
		bg=clr;
		fg=create_color(clr,3);
	}
	else
	{
		fg=clr;
		bg=create_color(clr,3);
	}
	switch (fontdata[entry][SF_TYPE])
	{
	case SF_BITMAP1:
		for (h=0;h<fontheight;h++)
		{
			for (w=0;w<fontwidth;w++)
			{
				if((fontdata[entry][h+SF_DATA] & (1<<(7-w)))!=0)
				{
					setColor(bg);
					drawLine(x+w+3,y+h+3,x+w+3,y+h+3);
					setColor(fg);
					drawLine(x+w+2,y+h+3,x+w+2,y+h+3);
				}
			}
		}
		break;
	case SF_BITMAP:
		for (h=0;h<fontheight;h++)
		{
			for (w=0;w<fontwidth;w++)
			{
				if((fontdata[entry][h+SF_DATA] & (1<<(7-w)))!=0)
				{
					fillRect(x+(int)(w*dw),
					y+(int)(h*dh),
					((int)((w+1)*dw))-(int)(w*dw),
					((int)((h+1)*dh))-(int)(h*dh),fg);
				}
			}
		}
		break;
	case SF_CIRCLE:
		for(j=SF_DATA;j<12;j++)
		{
			int xw,xh;
			w  = (fontdata[entry][j]&0xF000)>>12;
			h  = (fontdata[entry][j]&0x0F00)>>8;
			xw = (fontdata[entry][j]&0x00F0)>>4;
			xh = (fontdata[entry][j]&0x000F);
			if(!w && !h &&! xw && !xh)
				break;
			if(dw>dh)
				dw=dh;
			int x0=(int)(x+(w-xw)*dw);
			int y0=(int)(y+(h-xh)*dw);
			CRect rec(x0,y0,x0+(int)(xw*2*dw),y0+(int)(xw*2*dw));			
			pBitmap->Elipse(&rec,fg.value());
		}
		break;
	case SF_FILLRECT:
		switch(c)
		{
		case 181:
			i=304;
			break;
		case 182:
			i=179;
			break;
		case 183:
			i=303;
			break;
		case 184:
			i=304;
			break;
		case 185:
			i=180;
			break;
		case 186:
			i=179;
			break;
		case 187:
			i=191;
			break;
		case 188:
			i=217;
			break;
		case 189:
			i=301;
			break;
		case 190:
			i=304;
			break;
		case 198:
			i=302;
			break;
		case 199:
			i=179;
			break;
		case 200:
			i=192;
			break;
		case 201:
			i=218;
			break;
		case 202:
			i=193;
			break;
		case 203:
			i=194;
			break;
		case 204:
			i=195;
			break;
		case 205:
			i=196;
			break;
		case 206:
			i=197;
			break;
		case 207:
			i=196;
			break;
		case 208:
			i=301;
			break;
		case 209:
			i=196;
			break;
		case 210:
			i=303;
			break;
		case 211:
			i=301;
			break;
		case 212:
			i=302;
			break;
		case 213:
			i=302;
			break;
		case 214:
			i=303;
			break;
		case 215:
			i=179;
			break;
		case 216:
			i=196;
			break;
		default:
			i=0;
			break;
		}
		if(clr.value()!=0)
		{
			for(int j=SF_DATA;j<12;j++)
			{
				int     xw,xh;

				w  = (fontdata[entry][j]&0xF000)>>12;
				h  = (fontdata[entry][j]&0x0F00)>>8;
				xw = (fontdata[entry][j]&0x00F0)>>4;
				xh = (fontdata[entry][j]&0x000F);

				if(!w && !h &&! xw && !xh)
					break;
				
				int width  = ((int)((w+xw)*dw))-(int)(w*dw);
				int height = ((int)((h+xh)*dh))-(int)(h*dh);
				if(w+xw<8)
				{
					if(h==0)
						fillRect(x+(int)(w*dw)+2,y+(int)(h*dh),width,height,bg);
					else
						fillRect(x+(int)(w*dw)+2,y+(int)(h*dh)+2,width,height,bg);
				}
				if(h+xh<8)
				{
					if(w==0)
						fillRect(x+(int)(w*dw),y+(int)(h*dh)+2,width,height,bg);
					else
						fillRect(x+(int)(w*dw)+2,y+(int)(h*dh)+2,width,height,bg);
				}				
			}
		}
		for(j=SF_DATA;j<12;j++)
		{
			int xw,xh;
			w  = (fontdata[entry][j]&0xF000)>>12;
			h  = (fontdata[entry][j]&0x0F00)>>8;
			xw = (fontdata[entry][j]&0x00F0)>>4;
			xh = (fontdata[entry][j]&0x000F);
			if(w==0 && h==0 && xw==0 && xh==0)
				break;
			int width  = ((int)((w+xw)*dw))-(int)(w*dw);
			int height = ((int)((h+xh)*dh))-(int)(h*dh);

			fillRect(x+(int)(w*dw),y+(int)(h*dh),width,height,fg);
		}

		if(i && clr.value()!=0)
		{
			draw_box(create_color(fg,2),i,x,y,cw,ch,1);
		}
		break;
	default:
		break;
	}
}
