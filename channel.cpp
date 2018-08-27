// channel.cpp: implementation of the channel class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <io.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#include "terminalView.h"
#include "channel.h"
#include "Termcap.h"
#include "Display.h"
#include "telnetIO.h"

#include <stdio.h>
static char *cvtin_[]=
{
	"k1",  "k2",  "k3",  "k4",  "k5",  "k6",  "k7",  "k8",
	"k9",  "k0",  "F1",  "F2",  "ku",  "kd",  "kl",  "kr",
	"kh",  "@7",  "kD",  "kP",  "kN",  "kI",
        "AF",  "AB",  "so",  "se"
};

CString path;
channel::channel(CTerminalView *main_form,int act)
{
	this->main_form=main_form;
    this->act=act;
	io=NULL;
    term=NULL;
//	connect=false;
	Title="";
	cvtin =(char **)calloc(sizeof cvtin_,1);
}

char *channel::Output_Code(int i)
{
        if(i>=0 && i<(int)(sizeof(cvtin_)/sizeof (char *)))
                return(cvtin[i]);
        return(NULL);
}

channel::~channel()
{
	if(io!=NULL)
		delete io;
    if(cvtin!=NULL)
		free(cvtin);
	if(term!=NULL)
		delete term;
}

void  channel::Write(CString str)
{
	if(io->connected)
    {
		io->Send(str);
    }
}

//--------------------------------------------------------------------------
void  channel::set_active(bool act)
{
	if(term!=NULL)
		term->set_active(act);
}

int get_arg_line(CString line,char *pattern,char *arg);
void  channel::Connect(CString url)
{
	int i,y,x=100,port;
	char str[1024],prot[64];
	char *trm;
	int proxy_type=0,charset=RUSSIAN_CHARSET,mode=866;

	get_arg_line(url,"ProxyType",str);
	proxy_type=atoi(str);
	get_arg_line(url,"Port",str);
	port=atoi(str);
	get_arg_line(url,"Lines",str);
	y=atoi(str);
	get_arg_line(url,"Charset",str);
	if(str)
	{
		if(!strcmp(str,"ANSI"))
		{
			charset=ANSI_CHARSET;
			mode=0;
		}
		else if(!strcmp(str,"Default"))
		{
			charset=DEFAULT_CHARSET;
			mode=0;
		}
/*
		else if(!strcmp(str,"Japanese"))
		{
			charset=SHIFTJIS_CHARSET;
			mode=0;
		}
		else if(!strcmp(str,"Chinese"))
		{
			charset=GB2312_CHARSET;
			mode=0; 
		}
*/
		else if(!strcmp(str,"EastEurope"))
		{
			charset=EASTEUROPE_CHARSET;
			mode=0;
		}
		else if(!strcmp(str,"Baltic"))
		{
			charset=BALTIC_CHARSET;
			mode=0;
		}
		else if(!strcmp(str,"Symbol"))
		{
			charset=SYMBOL_CHARSET;
			mode=0;
		}
		else if(!strcmp(str,"Cyrillic_koi8"))
		{
			charset=RUSSIAN_CHARSET;
			mode=8;
		}
		else if(!strcmp(str,"Cyrillic_cp866"))
		{
			charset=RUSSIAN_CHARSET;
			mode=866;
		}
	}
	get_arg_line(url,"Host",str);
	get_arg_line(url,"Prot",prot);
	get_arg_line(url,"Term",term_type);


	host=str;
	if(!strcmp(term_type,"Linux"))
		trm="linux";
	else if(!strcmp(term_type,"FreeBSD"))
		trm="cons25";
	else if(!strcmp(term_type,"VT220"))
		trm="vt220";
	else if(!strcmp(term_type,"ANSI"))
		trm="ansi";
	else    trm="applet";


	Termcap *termc;
	try
	{
		termc = new Termcap(trm,stat_buf);
	}
	catch(...)
	{
		goto EXCEPT;
	}
	for(i=0;i<(int)(sizeof(cvtin_)/sizeof (char *));i++)
	{
		if(!cvtin_[i])
		{
			cvtin[i]=NULL;
			continue;
		}
		cvtin[i] = termc->tgetstr(cvtin_[i]);
	}
	delete termc;
EXCEPT:
	if(term!=NULL)
		delete term;
	term=new Terminal(this,x,y,"");

	CRect rec;
	main_form->GetClientRect(&rec);
	term->CharSet=charset;
	term->rus=mode;
	term->setBounds(rec.Width()-4,rec.Height());

	term->set_active(true);
	io = new telnetIO(term,trm,"");

	if(!strcmp(prot,"telnet"))
	{
		if(port==0)
			port=23;
	}
	else if(!strcmp(prot,"http-telnet"))
	{
		if(port==0)
			port=80;
	}
	else if(!strcmp(prot,"CX5"))
	{
		if(port==0)
			port=8177;
	}
	else
		proxy_type=-1;

	if(proxy_type)
	{
		if(proxy_type<0)
			proxy_type=0;
		if(_access(path+"bin/SSHProxy.exe",0))
		{
			AfxMessageBox("File "+path+"bin/SSHProxy.exe not found.\nCan't use this protocol");
			return;
		}
		else
		{
			int l_port=2348; 

			STARTUPINFO StartInfo; 
			PROCESS_INFORMATION PInfo;

			if(port==0)
			{
				if(!strcmp(prot,"SSH"))
					port=22;
				else
					port=513;
			}
			memset(&StartInfo,0,sizeof StartInfo);
			sprintf(str,"%d",l_port);
			CreateProcess(path+"bin/SSHProxy.exe", str, NULL, NULL, 0, 0, NULL, NULL, &StartInfo, &PInfo);
			main_form->Terminal_PID[main_form->act]=PInfo.hProcess;
			io->Connect("127.1",l_port);

			sprintf(str,"%s -P %d -terminal:%s -geom:%dx%d",host,port,trm,term->w0,term->h0);
			if(!strcmp(prot,"rlogin"))
				strcat(str, " -rlogin");
			if(proxy_type)
			{
				char buf[512],host[128];
				get_arg_line(url,"ProxyPort",buf);
				int proxy_port=atoi(buf);
				get_arg_line(url,"ProxyHost",host);
				if(port!=0 && *host)
				{
					sprintf(buf," -proxy:%d;%d;%s",proxy_type,proxy_port,host);
					strcat(str,buf);
				}
			}
		
			strcat(str,"\n");
			Sleep(500);
			io->Send(str);
			return;
		}
	}
	io->Connect(host,port);
}

void  channel::Disconnect()
{
	if(io!=NULL)
	{
		io->disconnect();
		delete io;
	}
	io=NULL;
}
