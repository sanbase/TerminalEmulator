// channel.h: interface for the channel class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHANNEL_H__F6B1F3E7_2895_4362_9F21_502A0F69B5AF__INCLUDED_)
#define AFX_CHANNEL_H__F6B1F3E7_2895_4362_9F21_502A0F69B5AF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "String.h"
class CTerminalView;
class telnetIO;
class Terminal;

#define TBUFSIZE  4096

class channel
{
	friend class CTerminalView;
	friend class CConteXtView;
	friend class Terminal;
	friend class Loader;
public:
	channel(CTerminalView *main_form,int);
	~channel();
//	void  left(TObject *Sender);
	void  Connect(CString url);
	void  Disconnect();
	Terminal *term;
	CString host;
	CString Title;
	bool update[256];
	void  Write(CString str);
	int port;
    int l,h;
//    CString term_name;
    CTerminalView *main_form;
	void  set_active(bool act);
	char* Output_Code(int i);
    char term_type[64];
private:
    char  stat_buf[TBUFSIZE];
//	bool connect;
	telnetIO *io;
    char **cvtin;
    int act;
};

#endif // !defined(AFX_CHANNEL_H__F6B1F3E7_2895_4362_9F21_502A0F69B5AF__INCLUDED_)
