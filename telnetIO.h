// telnetIO.h: interface for the telnetIO class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TELNETIO_H__886A9500_A19E_40FD_B07D_137E409266EF__INCLUDED_)
#define AFX_TELNETIO_H__886A9500_A19E_40FD_B07D_137E409266EF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//#include "string.h"
#include "Display.h"
#include "TelnetSocket.h"

#include <process.h>

#define STATE_DATA	        0
#define STATE_IAC	        1
#define STATE_IACSB	        2
#define STATE_IACWILL	        3
#define STATE_IACDO	        4
#define STATE_IACWONT	        5
#define STATE_IACDONT	        6
#define STATE_IACSBIAC	        7
#define STATE_IACSBDATA	        8
#define STATE_IACSBDATAIAC      9

#define SE    240
#define NOP   241
#define DM    242
#define BRK   243
#define IP    244
#define AO    245
#define AYT   246
#define EC    247
#define EL    248
#define GA    249
#define SB    250
#define WILL  251
#define WONT  252
#define DO    253
#define DONT  254
#define IAC   255
#define EOR   239


#define TELOPT_ECHO   1  /* echo on/off */
#define TELOPT_EOR   25  /* end of record */
#define TELOPT_NAWS  31  /* NA-WindowSize*/
#define TELOPT_TTYPE 24  /* terminal type */

#define TELQUAL_IS    0
#define TELQUAL_SEND  1

#define END_PTR 32

typedef unsigned char byte;
class TelnetSocket;
class TBitmap;
class CImage;

struct ImageHash
{
	CString *name;
	CImage *image;
};

class Loader
{
protected:
       void __fastcall Execute(void);
private:
	Terminal *term;
	CString name;
    CString des;
//    CBitmap **img;
	int x0;
	int y0;
	int x1;
	int y1;
	int transparent;
public:
	Loader(Terminal *term,CString Name_Image,CString des,int x0,int y0,int x1,int y1,int transparent);
    void Run(CDC *);
};

class telnetIO
{
		friend class TelnetSocket;
protected:
    void  Execute(void);
private:
	TelnetSocket *m_hSocket;
	Terminal *term;

	byte neg_state;
	byte sentDX[256];
	byte receivedWX[256];
	byte receivedDX[256];
	byte sentWX[256];
	byte current_sb;

	int Orandtbl[32];
	int Irandtbl[32];
    CString pubkey;
	byte I_crypt;
	byte O_crypt;
	int Ofptr;
	int Irptr;
	int Ifptr;
	int Orptr;
	int rand_deg;
	int rand_sep;

	int good_rand (int x);
	int O_Rand();
	int I_Rand();
	void Srand(int x);
	int O_ini();
	int I_ini();

	int Send(byte *,int);
	int Send(byte);
	void handle_sb(byte type, byte *sbdata, int sbcount);
	int negotiate(byte *in, int count);
	byte buf[4096];
	int port;
	CString secure;
    CString terminal;
public:
	bool connected;
	telnetIO(Terminal *,CString, CString);
	virtual ~telnetIO(void);
	int Connect(CString address, int port);
	void disconnect();
	int Send(CString);
	void ProcessMessage(TelnetSocket * pSock);
	CString m_host;
};


#endif // !defined(AFX_TELNETIO_H__886A9500_A19E_40FD_B07D_137E409266EF__INCLUDED_)
