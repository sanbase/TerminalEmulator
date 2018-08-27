// telnetIO.cpp: implementation of the telnetIO class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "telnetIO.h"
#include "Display.h"
#include <Afxsock.h>
#include <afxinet.h>

//#include <iostream.h>
#include "terminalView.h"
#include "terminalDoc.h"
#include "TBitmap.h"
#include "channel.h"
#include "CImage.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CString path;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
telnetIO::telnetIO(Terminal *term, CString terminal, CString secure)
{
	this->term=term;
    this->terminal=terminal;
	I_crypt  = 0;
	O_crypt  = 0;
	Ofptr    = 4;
	Irptr    = 1; 
	Ifptr    = 4;
	Orptr    = 1;
	rand_deg = 31;
	rand_sep = 3;
    this->secure=secure;
    WORD wVersionRequested;
	WSADATA wsaData;
	wVersionRequested = MAKEWORD( 2, 0 );
	WSAStartup( wVersionRequested, &wsaData );
	m_hSocket=new TelnetSocket(this);

	//create the socket and hook up to the host
	if(m_hSocket== NULL)
		AfxMessageBox("Could not create new socket",MB_OK);
}

telnetIO::~telnetIO()
{
        disconnect();
		delete m_hSocket;
}

int  telnetIO::good_rand (int x)
{
        int hi, lo;

        hi = x / 127773;
        lo = x % 127773;
        x = 16807 * lo - 2836 * hi;
        if (x <= 0)
                x += 0x7fffffff;
        return (x);
}

int  telnetIO::O_Rand()
{
        int i;
        int f, r;

        f = Ofptr;
        r = Orptr;
        Orandtbl[f]+=Orandtbl[r];
        i = (Orandtbl[f] >> 1) & 0x7fffffff;
        if (++f >= END_PTR)
        {
                f = 1;
                ++r;
        }
        else if (++r >= END_PTR)
                r = 1;
        Ofptr = f;
        Orptr = r;
        return(i);
}

int  telnetIO::I_Rand()
{
        int i;
        int f, r;

        f = Ifptr;
        r = Irptr;
        Irandtbl[f]+=Irandtbl[r];
        i = (Irandtbl[f] >> 1) & 0x7fffffff;
        if (++f >= END_PTR)
        {
                f = 1;
                ++r;
        }
        else if (++r >= END_PTR)
                r = 1;
        Ifptr = f;
        Irptr = r;
        return(i);
}

void  telnetIO::Srand(int x)
{
        Orandtbl[0] =Irandtbl[0] = x;
		int i;
        for (i = 0; i < rand_deg-1; i++)
                Orandtbl[i+1] = Irandtbl[i+1] = good_rand(Irandtbl[i]);
        Ofptr = Ifptr = rand_sep;
        Orptr = Irptr = 0;
        for (i = 0; i < 10 * rand_deg; i++)
        {
                I_Rand();
                O_Rand();
        }
}

int  telnetIO::Connect(CString address, int port)
{
	BOOL res=m_hSocket->Create();
	if(res == TRUE)
	{
		m_hSocket->AsyncSelect(FD_READ | FD_WRITE | FD_CLOSE | FD_CONNECT | FD_OOB);
	}
	else
	{
		delete m_hSocket;
		m_hSocket = NULL;
		return(-1);
	}
	m_host=address;
	if(m_hSocket->Connect(address,port)==false)
	{
			char *err;
			switch(m_hSocket->GetLastError())
			{
			case WSANOTINITIALISED:
				err="A successful AfxSocketInit must occur before using this API.";
				break;
			case WSAENETDOWN:
				err="The Windows Sockets implementation detected that the network subsystem failed.";
				break;
			case WSAEADDRINUSE:
				err="The specified address is already in use.";
				break;
			case WSAEINPROGRESS:
				err="A blocking Windows Sockets call is in progress.";
				break;
			case WSAEADDRNOTAVAIL:
				err="The specified address is not available from the local machine.";
				break;
			case WSAEAFNOSUPPORT:
				err="Addresses in the specified family cannot be used with this socket.";
				break;
			case WSAECONNREFUSED:
				err="The attempt to connect was rejected.";
				break;
			case WSAEDESTADDRREQ:
				err="A destination address is required.";
				break;
			case WSAEFAULT:
				err="The nSockAddrLen argument is incorrect.";
				break;
			case WSAEINVAL:
				err="Invalid host address.";
				break;
			case WSAEISCONN:
				err="The socket is already connected.";
				break;
			case WSAEMFILE:
				err="No more file descriptors are available.";
				break;
			case WSAENETUNREACH:
				err="The network cannot be reached from this host at this time.";
				break;
			case WSAENOBUFS:
				err="No buffer space is available. The socket cannot be connected.";
				break;
			case WSAENOTSOCK:
				err="The descriptor is not a socket.";
				break;
			case WSAETIMEDOUT:
				err="Attempt to connect timed out without establishing a connection.";
				break;
			case WSAEWOULDBLOCK:
				Sleep(100);
/*
					err="The socket is marked as nonblocking and the connection cannot be completed immediately.";
					break;
*/
				goto NEXT_STEP;
			default:
				err="Unknown error";
			}
			AfxMessageBox(err);
			return(-1);
	}
NEXT_STEP:
	{
//		term->main_form->GetDocument()->SetTitle(address);
		if(port==80)
		{
			Sleep(1000);
			if(secure.GetLength()>32)
			{
				Send("CRYPTO\n");
				O_crypt=1;
				I_crypt=1;
				pubkey=secure;
				int i=I_ini();  // read o_key
				i+=O_ini();     // send i_key
				Srand(i);
			}
			else
			{
				int i=Send("TELNET\n");
				O_crypt=0;
				I_crypt=0;
			}
		}
		connected=true;
		neg_state = 0;
		this->port=port;
	}
	return res;
}

void  telnetIO::disconnect()
{
        if(m_hSocket!=NULL && connected==true)
        {
				m_hSocket->Close();
//                closesocket(m_hSocket);
        }
}

#include <time.h>
void telnetIO::ProcessMessage(TelnetSocket * pSock)
{
	if(connected==false)
		return;
	int nBytes = pSock->Receive(buf,sizeof buf);
	if(nBytes != SOCKET_ERROR)
	{
        int size=negotiate(buf,nBytes);
		if(size>0)
		{
			EnterCriticalSection(&term->main_form->m_DrawSynchro);
			term->putStringLen((char *)buf,size);
			LeaveCriticalSection(&term->main_form->m_DrawSynchro);
//			term->main_form->paint();
		}
	}
	else
	{
		char *err;
		switch(pSock->GetLastError())
		{
		case WSANOTINITIALISED:
			err="A successful AfxSocketInit must occur before using this API.";
			break;
		case WSAENETDOWN:
			err="The Windows Sockets implementation detected that the network subsystem failed.";
			break;
		case WSAENOTCONN:
			err="The socket is not connected.";
			break;
		case WSAEINPROGRESS:
			err="A blocking Windows Sockets operation is in progress.";
			break;
		case WSAENOTSOCK:
			err="The descriptor is not a socket.";
			break;
		case WSAEOPNOTSUPP:
			err="MSG_OOB was specified, but the socket is not of type SOCK_STREAM.";
			break;
		case WSAESHUTDOWN:
			err="The socket has been shut down; it is not possible to call Receive on a socket after ShutDown has been invoked with nHow set to 0 or 2.";
			break;
		case WSAEWOULDBLOCK:
			err="The socket is marked as nonblocking and the Receive operation would block.";
			break;
		case WSAEMSGSIZE:
			err="The datagram was too large to fit into the specified buffer and was truncated.";
			break;
		case WSAEINVAL:
			err="The socket has not been bound with Bind.";
			break;
		case WSAECONNABORTED:
			err="The virtual circuit was aborted due to timeout or other failure.";
			break;
		case WSAECONNRESET:
			err="The virtual circuit was reset by the remote side. ";
			break;
		default:
			err="Unknown error";
			break;
		}
		AfxMessageBox(err);
	}
}

int  telnetIO::O_ini()
{
        O_crypt=2;
        int i=(int)((double)(0x3fffffff)*rand());
        if(i<0)
        {
                i=-i;
        }
        char str[64];
        sprintf(str,"%d",i);
//        Communicator c = new Communicator(pubkey);
//        byte *b=c.doSendRSAMessage(str);
//        os.write(b);
//        os.flush();
        return(i);
}

int  telnetIO::I_ini()
{
        int i=0;
		int count = m_hSocket->Receive(buf,sizeof(buf));
        if(count>0)
        {
                i=atoi((char *)buf);
                I_crypt=2;
        }
        return(i);
}

int  telnetIO::Send(byte *buf,int len)
{
/*
        if(connected==false)
        {
                return(0);
        }
*/
        if(O_crypt>0)
        {
                for(int i=0;i<len;i++)
                {
                        int j=buf[i];
                        int r=O_Rand()%0xff;
                        j-=r;
                        buf[i]=(byte)j;
                }
        }
		int l=m_hSocket->Send(buf,len);
		if(l==SOCKET_ERROR)
		{
			char *err;
			switch(m_hSocket->GetLastError())
			{
				case WSANOTINITIALISED:
					err="A successful AfxSocketInit must occur before using this API.";
					break;
				case WSAENETDOWN:
					err="The Windows Sockets implementation detected that the network subsystem failed.";
					break;
				case WSAEACCES:
					err="The requested address is a broadcast address, but the appropriate flag was not set.";
					break;
				case WSAEINPROGRESS:
					err="A blocking Windows Sockets operation is in progress.";
					break;
				case WSAEFAULT:
					err="The lpBuf argument is not in a valid part of the user address space.";
					break;
				case WSAENETRESET:
					err="The connection must be reset because the Windows Sockets implementation dropped it.";
					break;
				case WSAENOBUFS:
					err="The Windows Sockets implementation reports a buffer deadlock.";
					break;
				case WSAENOTCONN:
					err="The socket is not connected.";
					break;
				case WSAENOTSOCK:
					err="The descriptor is not a socket.";
					break;
				case WSAEOPNOTSUPP:
					err="MSG_OOB was specified, but the socket is not of type SOCK_STREAM.";
					break;
				case WSAESHUTDOWN:
					err="The socket has been shut down; it is not possible to call Send on a socket after ShutDown has been invoked with nHow set to 1 or 2.";
					break;
				case WSAEWOULDBLOCK:
					err="The socket is marked as nonblocking and the requested operation would block.";
					break;
				case WSAEMSGSIZE:
					err="The socket is of type SOCK_DGRAM, and the datagram is larger than the maximum supported by the Windows Sockets implementation.";
					break;
				case WSAEINVAL:
					err="The socket has not been bound with Bind.";
					break;
				case WSAECONNABORTED:
					err="The virtual circuit was aborted due to timeout or other failure.";
					break;
				case WSAECONNRESET:
					err="The virtual circuit was reset by the remote side.";
					break;
				default:
					err="Unknown error";
					break;
			}
			AfxMessageBox(err);
		}
		return(l);
}

int  telnetIO::Send(CString s)
{
        int len=s.GetLength();
		const char *a=s;
        return(Send((unsigned char *)a,len));
}

int  telnetIO::Send(byte b)
{
        return(Send(&b,1));
}

void  telnetIO::handle_sb(unsigned char type, unsigned char *sbdata, int sbcount)
{
        switch (type)
        {
                case TELOPT_TTYPE:
                if (sbcount>0 && sbdata[0]==TELQUAL_SEND)
                {
                        byte buf[2]={255,250};
                        Send(buf,2);
                        Send(TELOPT_TTYPE);
                        Send(TELQUAL_IS);
						int len=terminal.GetLength();
                        Send((byte *)((const char *)terminal),len);
                        buf[1]=240;
                        Send(buf,2);
                }
        }
}

int  telnetIO::negotiate(byte *in,int count)
{
        byte *nbuf = (byte *)calloc(count,1);
        byte *sbbuf= (byte *)calloc(count,1);
        byte *sendbuf = (byte *)calloc(count,1);
        byte b,reply=0;
        int  sbcount = 0;
        int boffset = 0, noffset = 0;

        int width=term->scr->Width();
        int height=term->scr->Height();

        if(I_crypt>0)
        {
                for(int i=0;i<count;i++)
                {
                        int j=in[i];
                        int r;
                        if(I_crypt>1)
                        {
                                r=I_Rand();
                        }
                        else
                        {
                                r=0;
                        }
                        r%=0xff;
                        if(j<0)
                        {
                                j=0x100+j;
                        }
                        j-=r;
                        if(j<0)
                        {
                                j=0x100+j;
                        }
                        in[i]=(byte)j;
                }
        }
        while(boffset < count)
        {
                b=in[boffset++];
                switch (neg_state)
                {
			case STATE_DATA:
				if (b==IAC)
				{
					neg_state = STATE_IAC;
				}
				else
				{
					nbuf[noffset++]=b;
				}
				break;
			case STATE_IAC:
				switch (b)
				{
				case IAC:
					neg_state = STATE_DATA;
					nbuf[noffset++]=IAC;
					break;
				case WILL:
					neg_state = STATE_IACWILL;
					break;
				case WONT:
					neg_state = STATE_IACWONT;
					break;
				case DONT:
					neg_state = STATE_IACDONT;
					break;
				case DO:
					neg_state = STATE_IACDO;
					break;
				case EOR:
					neg_state = STATE_DATA;
					break;
				case SB:
					neg_state = STATE_IACSB;
					sbcount = 0;
					break;
				default:
					neg_state = STATE_DATA;
					break;
				}
				break;
			case STATE_IACWILL:
				switch(b)
				{
				case TELOPT_ECHO:
					reply = DO;
//					vec = new Vector(2);
//					vec.addElement("NOLOCALECHO");
//					peer.notifyStatus(vec);
					break;
				case TELOPT_EOR:
					reply = DO;
					break;
				default:
					reply = DONT;
					break;
				}
				if(reply != sentDX[b+128] || WILL != receivedWX[b+128])
				{
					sendbuf[0]=IAC;
					sendbuf[1]=reply;
					sendbuf[2]=b;
					Send(sendbuf,3);
					sentDX[b+128] = reply;
					receivedWX[b+128] = WILL;
				}
				neg_state = STATE_DATA;
				break;
			case STATE_IACWONT:
				switch(b)
				{
				case TELOPT_ECHO:

//					vec = new Vector(2);
//					vec.addElement("LOCALECHO");
//					peer.notifyStatus(vec);
					reply = DONT;
					break;
				case TELOPT_EOR:
					reply = DONT;
					break;
				default:
					reply = DONT;
					break;
				}
				if(reply != sentDX[b+128] || WONT != receivedWX[b+128])
				{
					sendbuf[0]=IAC;
					sendbuf[1]=reply;
					sendbuf[2]=b;
					Send(sendbuf,3);
					sentDX[b+128] = reply;
					receivedWX[b+128] = WILL;
				}
				neg_state = STATE_DATA;
				break;
			case STATE_IACDO:
				switch (b)
				{
				case TELOPT_TTYPE:
					reply = WILL;
					break;
				case TELOPT_NAWS:
					receivedDX[b] = DO;
					reply = WILL;
					sentWX[b] = WILL;
					sendbuf[0]=IAC;
					sendbuf[1]=WILL;
					sendbuf[2]=TELOPT_NAWS;
					Send(sendbuf,3);
					Send(IAC);
					Send(SB);
					Send(TELOPT_NAWS);
					Send((byte) (width >> 8));
					Send((byte) (width & 0xff));
					Send((byte) (height >> 8));
					Send((byte) (height & 0xff));
					Send(IAC);
					Send(SE);
					break;
				default:
					reply = WONT;
					break;
				}
				if(reply != sentWX[128+b] || DO != receivedDX[128+b])
				{
					sendbuf[0]=IAC;
					sendbuf[1]=reply;
					sendbuf[2]=b;
					Send(sendbuf,3);
					sentWX[b+128] = reply;
					receivedDX[b+128] = DO;
				}
				neg_state = STATE_DATA;
				break;
			case STATE_IACDONT:
				switch (b)
				{
				case TELOPT_NAWS:
					reply	= WONT;
					break;
				default:
					reply	= WONT;
					break;
				}
				if(reply!= sentWX[b+128]||DONT!= receivedDX[b+128])
				{
					Send(IAC);
					Send(reply);
					Send(b);
					sentWX[b+128]		= reply;
					receivedDX[b+128]	= DONT;
				}
				neg_state = STATE_DATA;
				break;
			case STATE_IACSBIAC:
				if (b == IAC)
				{
					sbcount		= 0;
					current_sb	= b;
					neg_state	= STATE_IACSBDATA;
				}
				else
				{
					neg_state	= STATE_DATA;
				}
				break;
			case STATE_IACSB:
				switch (b)
				{
				case IAC:
					neg_state = STATE_IACSBIAC;
					break;
				default:
					current_sb	= b;
					sbcount		= 0;
					neg_state	= STATE_IACSBDATA;
					break;
				}
				break;
			case STATE_IACSBDATA:
				switch (b)
				{
				case IAC:
					neg_state = STATE_IACSBDATAIAC;
					break;
				default:
					sbbuf[sbcount++] = b;
					break;
				}
				break;
			case STATE_IACSBDATAIAC:
				switch (b)
				{
				case IAC:
					neg_state = STATE_IACSBDATA;
					sbbuf[sbcount++] = IAC;
					break;
				case SE:
					handle_sb(current_sb,sbbuf,sbcount);
					current_sb	= 0;
					neg_state	= STATE_DATA;
					break;
				case SB:
					handle_sb(current_sb,sbbuf,sbcount);
					neg_state	= STATE_IACSB;
					break;
				default:
					neg_state	= STATE_DATA;
					break;
				}
				break;
                        default:
                                neg_state = STATE_DATA;
                                break;
                }
        }
        free(sbbuf);
        free(sendbuf);
        memcpy(in,nbuf, noffset);

        free(nbuf);
        return noffset;
}

void LoadPicture(char *buf, int dwFileSize, CBitmap *img)
{
        IPicture *pic;

        HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, dwFileSize);
        LPVOID pvData = GlobalLock(hGlobal);

        memcpy((char *)pvData,buf,dwFileSize);
        GlobalUnlock(hGlobal);

        LPSTREAM pstm = NULL;
        CreateStreamOnHGlobal(hGlobal, true, &pstm);
        ::OleLoadPicture(pstm, dwFileSize, false, IID_IPicture, (LPVOID *)&pic);
        if(pic!=NULL)
        {
                HBITMAP hbm0=0;
                pic->get_Handle((OLE_HANDLE*)&hbm0);
				img->Attach(hbm0);
        }
        pstm->Release();
        GlobalFree(hGlobal);
}

Loader::Loader(Terminal *term, CString name, CString des,int x0,int y0,int x1,int y1,int transparent)
{
	this->term=term;
	this->name=name;
    this->des=des;
	this->x0=x0;
	this->y0=y0; 
	this->x1=x1;
	this->y1=y1;
	this->transparent=transparent;
}

extern ImageHash *image_hash;
extern int num_images;

void  Loader::Run(CDC *dc)
{
    CImage *Image = new CImage();
	BOOL res=false;


    if(term->channels->host!="127.0.0.1" && term->channels->host!="127.1" && term->channels->host!="localhost")
		res=Image->LoadFromURL(term->channels->host,name,dc);
	else
	{
		res=Image->LoadImage(path+name,dc);
	}
	if(Image->FileSize==0 || res==false)
	{
		delete Image;
		return;
	}

	if(Image->FileSize<1000)
	{
		image_hash =(struct ImageHash *)realloc(image_hash,++num_images*sizeof (struct ImageHash));
		image_hash[num_images-1].name=new CString(name);
		image_hash[num_images-1].image=Image;
	}
    if(x0!=-1)
	{
		term->putObject(X_IMAGE,x0,y0,x1,y1,transparent,0,0,0,Image,des);
	}
}




/*
#include <IImgCtx.h> // IImgCtx declaration

//@//////////////////////////////////////////////////////////////////////////
//	CImgCtxImage
class ATL_NO_VTABLE CImgCtxImage
{

	// IImgCtx object used to load and render images
	CComPtr<IImgCtx> m_pImage;
public:
	// Overrides
	static LPCTSTR GetMethodName()
	{
		return _T("ImgCtx");
	}

	void Destroy()
	{
		m_pImage.Release();
	}

	// Implementation
	HRESULT DrawImg(HDC hdc, RECT& rcBounds)
	{
		if (m_pImage)
		{
			// Check download state
			DWORD dwState = 0;
			HRESULT hr = m_pImage->GetStateInfo(&dwState, NULL, true);
			if (SUCCEEDED(hr))
			{
				if (IMGLOAD_LOADING & dwState)
				{
					// Still loading - wait 50 msec and request again
					::DrawText(hdc, _T("Loading, please wait..."), -1, &rcBounds, DT_SINGLELINE);
					::Sleep(50);
//					Invalidate(false);

					hr = S_FALSE;
				}
				else if (IMGLOAD_COMPLETE & dwState)
				{
					// Download successfully complete

					hr = m_pImage->Draw(
						hdc,				// [in] Handle of device context on which to render the image
						&rcBounds			// [in] Position and dimensions
						);

				}
				else
				{
					// Download failed
					hr = E_UNEXPECTED;
				}
			}
			return hr;
		}

		return E_UNEXPECTED;
	}
	HRESULT Load(LPCTSTR szFile, CBitmap *bmp)
	{
		// Create IImgCtx object
		HRESULT hr = ::CoCreateInstance(CLSID_IImgCtx, NULL, CLSCTX_ALL, IID_IImgCtx, (void**)&m_pImage);
		if (SUCCEEDED(hr))
		{
			// Load URL
			DWORD dwState;
			USES_CONVERSION;
			hr = m_pImage->Load(T2COLE(szFile),0);// URL, Flags and preffered color format
CHECK:
			dwState = 0;
			HRESULT hr = m_pImage->GetStateInfo(&dwState, NULL, true);
			if (SUCCEEDED(hr))
			{
				if (IMGLOAD_LOADING & dwState)
				{
					// Still loading - wait 50 msec and request again
					::Sleep(50);
					goto CHECK;
				}
				else if (IMGLOAD_COMPLETE & dwState)
				{
						HDC hdc = (HDC)bmp->GetSafeHandle();
					// Download successfully complete
						m_pImage->Draw(
						hdc,				// [in] Handle of device context on which to render the image
						&rcBounds			// [in] Position and dimensions
						);
				}
				else
				{
					// Download failed
					hr = E_UNEXPECTED;
				}
			}
		}

		return hr;
	}
};
*/
