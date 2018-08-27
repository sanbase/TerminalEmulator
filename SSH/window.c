#include <windows.h> 
#include <imm.h> 
#include <commctrl.h> 
#include <richedit.h> 
#include <mmsystem.h> 
#ifndef AUTO_WINSOCK 
#ifdef WINSOCK_TWO 
#include <winsock2.h> 
#else 
#include <winsock.h> 
#endif 
#endif 

#ifndef NO_MULTIMON 
#if WINVER < 0x0500 
#define COMPILE_MULTIMON_STUBS 
#include <multimon.h> 
#endif 
#endif 

#include <stdio.h> 
#include <stdlib.h> 
#include <ctype.h> 
#include <time.h> 
#include <assert.h> 

#include <io.h>
#include <sys\stat.h>
#include <fcntl.h>

#define PUTTY_DO_GLOBALS	       /* actually _define_ globals */ 
#include "putty.h" 
#include "terminal.h" 
#include "winstuff.h" 
#include "storage.h" 
#include "win_res.h" 
#include "dialog.h"

#define IDM_SHOWLOG   0x0010 
#define IDM_NEWSESS   0x0020 
#define IDM_DUPSESS   0x0030 
#define IDM_RECONF    0x0040 
#define IDM_CLRSB     0x0050 
#define IDM_RESET     0x0060 
#define IDM_HELP      0x0140 
#define IDM_ABOUT     0x0150 
#define IDM_SAVEDSESS 0x0160 
#define IDM_COPYALL   0x0170 
#define IDM_FULLSCREEN	0x0180 

#define IDM_SESSLGP   0x0250	       /* log type printable */ 
#define IDM_SESSLGA   0x0260	       /* log type all chars */ 
#define IDM_SESSLGE   0x0270	       /* log end */ 

#define IDM_SPECIAL_MIN 0x0400 
#define IDM_SPECIAL_MAX 0x0800 

#define IDM_SAVED_MIN 0x1000 
#define IDM_SAVED_MAX 0x2000 

#define WM_IGNORE_CLIP (WM_XUSER + 2) 
#define WM_FULLSCR_ON_MAX (WM_XUSER + 3) 
#define WM_AGENT_CALLBACK (WM_XUSER + 4) 

/** Needed for Chinese support and apparently not always defined. */ 
#ifndef VK_PROCESSKEY 
#define VK_PROCESSKEY 0xE5 
#endif 

/** Mouse wheel support. */ 
#ifndef WM_MOUSEWHEEL 
#define WM_MOUSEWHEEL 0x020A	       /* not defined in earlier SDKs */ 
#endif 
#ifndef WHEEL_DELTA 
#define WHEEL_DELTA 120 
#endif 

static Mouse_Button translate_button(Mouse_Button button); 
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM); 
static int TranslateKey(UINT message, WPARAM wParam, LPARAM lParam, 
unsigned char *output); 
static void cfgtopalette(void); 
static void systopalette(void); 
static void init_palette(void); 
static void init_fonts(int, int); 
static void another_font(int); 
static void deinit_fonts(void); 
static void set_input_locale(HKL); 

static int is_full_screen(void); 
static void make_full_screen(void); 
static void clear_full_screen(void); 
static void flip_full_screen(void); 

/** Window layout information */ 
static void reset_window(int); 
static int extra_width, extra_height; 
static int font_width, font_height, font_dualwidth; 
static int offset_width, offset_height; 
static int was_zoomed = 0; 
static int prev_rows, prev_cols; 
char *do_select(SOCKET skt, int startup);
static int pending_netevent = 0; 
static WPARAM pend_netevent_wParam = 0; 
static LPARAM pend_netevent_lParam = 0; 
static void enact_pending_netevent(void); 
static void flash_window(int mode); 
static void sys_cursor_update(void); 
static int is_shift_pressed(void); 
static int get_fullscreen_rect(RECT * ss); 

static time_t last_movement = 0; 

static int caret_x = -1, caret_y = -1; 
static int kbd_codepage; 

static void *ldisc; 
static Backend *back; 
static void *backhandle; 

static struct unicode_data ucsdata; 
static int session_closed; 

static const struct telnet_special *specials; 
static int specials_menu_position; 

Config cfg;			       /* exported to windlg.c */

static SOCKET m_sockfd;
static int local_port;
static char remote_addr[256];

extern struct sesslist sesslist;       /* imported from windlg.c */


struct agent_callback
{
	void (*callback)(void *, void *, int); 
	void *callback_ctx; 
	void *data; 
	int len; 
}; 

#define FONT_NORMAL 0 
#define FONT_BOLD 1 
#define FONT_UNDERLINE 2 
#define FONT_BOLDUND 3 
#define FONT_WIDE	0x04 
#define FONT_HIGH	0x08 
#define FONT_NARROW	0x10 

#define FONT_OEM 	0x20 
#define FONT_OEMBOLD 	0x21 
#define FONT_OEMUND 	0x22 
#define FONT_OEMBOLDUND 0x23 

#define FONT_MAXNO 	0x2F 
#define FONT_SHIFT	5 
static HFONT fonts[FONT_MAXNO]; 
static LOGFONT lfont; 
static int fontflag[FONT_MAXNO]; 
static enum
{
	BOLD_COLOURS, BOLD_SHADOW, BOLD_FONT 
} 
bold_mode; 
static enum
{
	UND_LINE, UND_FONT 
} 
und_mode; 
static int descent; 

#define NCOLOURS 24 
static COLORREF colours[NCOLOURS]; 
static HPALETTE pal; 
static LPLOGPALETTE logpal; 
static RGBTRIPLE defpal[NCOLOURS]; 

static HWND hwnd; 

static HBITMAP caretbm; 

static int dbltime, lasttime, lastact; 
static Mouse_Button lastbtn; 

/** this allows xterm-style mouse handling. */ 
static int send_raw_mouse = 0; 
static int wheel_accumulator = 0; 

static char *window_name, *icon_name; 

static int compose_state = 0; 

static int wsa_started = 0; 

static UINT wm_mousewheel = WM_MOUSEWHEEL; 

/** Dummy routine, only required in plink. */ 
void ldisc_update(void *frontend, int echo, int edit) 
{ 
} 

long GetVSNumber()
{
	char NameBuffer[MAX_PATH];
	char SysNameBuffer[MAX_PATH];
	DWORD VSNumber=0;
	DWORD MCLength;
	DWORD FileSF;

	GetVolumeInformation("C:\\",NameBuffer, sizeof(NameBuffer),&VSNumber,&MCLength,&FileSF,SysNameBuffer,sizeof(SysNameBuffer));
	return(VSNumber);
}

void check_key()
{
	int fd,hi,lo,key=0,x,RN=0;
	DWORD k;	
	char buf[256];
	struct stat st;
	memset(buf,0,sizeof buf);

	if(!stat("C:/Program Files/UnixSpace/bin/UnixSpace.exe",&st))
	{
		if((fd=open("C:/Program Files/UnixSpace/etc/Terminal.key",O_RDONLY))<0)
			goto ASK;
		read(fd,buf,sizeof buf);
		close(fd);
		key=atoi((const char *)buf);
		goto ASK;
	}
	if(!stat("C:/Program Files/UnixSpace/bin/SSHProxy.exe",&st))
	{
		if((fd=open("C:/Program Files/UnixSpace/etc/SSHProxy.key",O_RDONLY))<0)                                         
			goto ASK;
	}
	else
	{
		MessageBox(NULL,"The file SSHProxy.exe should be in the C:\\Program Files\\UnixSpace\\bin directory.", "Error",MB_OK | MB_ICONEXCLAMATION);
		exit(0);
	}
	read(fd,buf,sizeof buf);
	close(fd);
	key=atoi((const char *)buf);
ASK:
	k=st.st_ctime+GetVSNumber(); 
	RN=x=k%673452;
	hi = x / 127773;
	lo = x % 127773;
	x = 16807 * lo - 2836 * hi;
	if (x <= 0)
		x += 0x7fffffff;
	x%=100000;
	if(key==x)
		return;
	sprintf(buf,"This is an unregistered program only for evaluation purposes.\nThe registration number of your copy is:%d",RN);
	MessageBox(NULL,buf, "Warning",MB_OK | MB_ICONEXCLAMATION);
}

int WINAPI WinMain(HINSTANCE inst, HINSTANCE prev, LPSTR cmdline, int show) 
{ 
	WORD winsock_ver; 
	WSADATA wsadata; 
	WNDCLASS wndclass; 
	MSG msg; 
	int guess_width, guess_height; 

	hinst = inst; 
	flags = FLAG_VERBOSE | FLAG_INTERACTIVE; 

	winsock_ver = MAKEWORD(1, 1); 
	if (WSAStartup(winsock_ver, &wsadata))
	{
		MessageBox(NULL, "Unable to initialise WinSock", "WinSock Error", MB_OK | MB_ICONEXCLAMATION); 
		return 1; 
	} 
	if (LOBYTE(wsadata.wVersion) != 1 || HIBYTE(wsadata.wVersion) != 1)
	{
		MessageBox(NULL, "WinSock version is incompatible with 1.1", "WinSock Error", MB_OK | MB_ICONEXCLAMATION); 
		WSACleanup(); 
		return 1; 
	} 

	wsa_started = 1; 
	/** WISHLIST: maybe allow config tweaking even if winsock not present? */ 
	sk_init(); 

	InitCommonControls(); 

		/** Ensure a Maximize setting in Explorer doesn't maximise the 
	     * config box. */ 
//	defuse_showwindow(); 

	if (!init_winver()) 
	{ 
		char *str = dupprintf("%s Fatal Error", appname); 
		MessageBox(NULL, "Windows refuses to report a version",  str, MB_OK | MB_ICONEXCLAMATION); 
		sfree(str); 
		return 1; 
	} 

		/** 
	     * If we're running a version of Windows that doesn't support 
	     * WM_MOUSEWHEEL, find out what message number we should be 
	     * using instead. 
	     */ 
	if (osVersion.dwMajorVersion < 4 || (osVersion.dwMajorVersion == 4 &&  osVersion.dwPlatformId != VER_PLATFORM_WIN32_NT)) 
		wm_mousewheel = RegisterWindowMessage("MSWHEEL_ROLLMSG"); 

	if (!prev)
	{
		wndclass.style = 0; 
		wndclass.lpfnWndProc = WndProc; 
		wndclass.cbClsExtra = 0; 
		wndclass.cbWndExtra = 0; 
		wndclass.hInstance = inst; 
		wndclass.hIcon = LoadIcon(inst, MAKEINTRESOURCE(IDI_MAINICON)); 
		wndclass.hCursor = LoadCursor(NULL, IDC_IBEAM); 
		wndclass.hbrBackground = NULL; 
		wndclass.lpszMenuName = NULL; 
		wndclass.lpszClassName = appname; 

		RegisterClass(&wndclass); 
	} 

	hwnd = NULL; 

	memset(&ucsdata, 0, sizeof(ucsdata)); 

	term = term_init(&cfg, &ucsdata, NULL); 
//	logctx = log_init(NULL, &cfg); 
//	term_provide_logctx(term, logctx); 

	cfgtopalette(); 

	     // Guess some defaults for the window size. This all gets 
	     // updated later, so we don't really care too much. However, we 
	     // do want the font width/height guesses to correspond to a 
	     // large font rather than a small one... 

	font_width = 10; 
	font_height = 20; 
	extra_width = 25; 
	extra_height = 28; 

	guess_width=100;
	guess_height=100;

	{ 
		int winmode = WS_OVERLAPPEDWINDOW | WS_VSCROLL; 
		int exwinmode = 0; 
		if (!cfg.scrollbar) 
			winmode &= ~(WS_VSCROLL); 
		if (cfg.resize_action == RESIZE_DISABLED) 
			winmode &= ~(WS_THICKFRAME | WS_MAXIMIZEBOX); 
		if (cfg.alwaysontop) 
			exwinmode |= WS_EX_TOPMOST; 
		if (cfg.sunken_edge) 
			exwinmode |= WS_EX_CLIENTEDGE; 
		hwnd = CreateWindowEx(exwinmode, appname, appname, winmode, CW_USEDEFAULT, CW_USEDEFAULT, guess_width, guess_height, NULL, NULL, inst, NULL); 
	} 

	init_fonts(0,0); 

	{ 
		RECT cr, wr; 
		GetWindowRect(hwnd, &wr); 
		GetClientRect(hwnd, &cr); 
		offset_width = offset_height = cfg.window_border; 
		extra_width = wr.right - wr.left - cr.right + cr.left + offset_width*2; 
		extra_height = wr.bottom - wr.top - cr.bottom + cr.top +offset_height*2; 
	} 
/*
	guess_width =   extra_width + font_width * term->cols; 
	guess_height = extra_height + font_height * term->rows; 
*/

	SetWindowPos(hwnd, NULL, 0, 0, guess_width, guess_height, SWP_NOMOVE | SWP_NOREDRAW | SWP_NOZORDER); 

	{ 
		char *bits; 
		int size = (font_width + 15) / 16 * 2 * font_height; 
		bits = snewn(size, char); 
		memset(bits, 0, size); 
		caretbm = CreateBitmap(font_width, font_height, 1, 1, bits); 
		sfree(bits); 
	} 
	CreateCaret(hwnd, caretbm, font_width, font_height); 
/*
	// Initialise the scroll bar. 
	{ 
		SCROLLINFO si; 

		si.cbSize = sizeof(si); 
		si.fMask = SIF_ALL | SIF_DISABLENOSCROLL; 
		si.nMin = 0; 
		si.nMax = term->rows - 1; 
		si.nPage = term->rows; 
		si.nPos = 0; 
		SetScrollInfo(hwnd, SB_VERT, &si, FALSE); 
	} 
*/
	     // Finally show the window! 
	show=SW_HIDE;
	ShowWindow(hwnd, show); 
	SetForegroundWindow(hwnd);  

	// Start up the telnet connection. 
	{ 
		const char *error; 
		char msg[1024], *title; 
		char *realhost; 

		{
			struct sockaddr_in serv_addr;
			struct sockaddr_in cli_addr;
			SOCKET sock_fd;
			WORD wVersionRequested;
			WSADATA wsaData; 
			int clilen = sizeof(cli_addr);
			int i,err;
			char *mess="Can't create socket";
			u_short localport=2348;

			if(cmdline!=NULL && *cmdline)
				localport=atoi(cmdline);

			wVersionRequested = MAKEWORD( 1, 1);

			err = WSAStartup( wVersionRequested, &wsaData );
			if ( err != 0 )
			{
			/// Tell the user that we could not find a usable 
			/// WinSock DLL.                                  
SOCK_ERR:
				MessageBox(hwnd, mess,"WinSock Error", MB_OK | MB_ICONEXCLAMATION);
				DestroyWindow(hwnd); 
				exit(0);
			}

			if( (m_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
			{
				switch(WSAGetLastError ())
				{
				case WSANOTINITIALISED:
					mess="A successful WSAStartup must occur before using this function.";
					break;
				case WSAENETDOWN:
					mess="The network subsystem or the associated service provider has failed.";
					break;
				case WSAEAFNOSUPPORT:
					mess="The specified address family is not supported. ";
					break;
				case WSAEINPROGRESS:
					mess="A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function. ";
					break;
				case WSAEMFILE:
					mess="No more socket descriptors are available.";
					break;
				case WSAENOBUFS:
					mess="No buffer space is available. The socket cannot be created. ";
					break;
				case WSAEPROTONOSUPPORT:
					mess="The specified protocol is not supported. ";
					break;
				case WSAEPROTOTYPE:
					mess="The specified protocol is the wrong type for this socket. ";
					break;
				case WSAESOCKTNOSUPPORT:
					mess="The specified socket type is not supported in this address family. ";
					break;
				default:
					mess="Unknown error";
					break;
				}
				goto SOCK_ERR;
			}

//		ReuseAddress();

			memset(&serv_addr,0,sizeof serv_addr);
			serv_addr.sin_family      = AF_INET;
			serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
			serv_addr.sin_port        = htons(localport);

			if( bind( m_sockfd,(struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
			{
				mess="Bind error";
				goto SOCK_ERR;
			}

			listen(m_sockfd,SOMAXCONN);

#ifdef _DEBUG
			{
				STARTUPINFO StartInfo; 
				PROCESS_INFORMATION PInfo;

				memset(&StartInfo,0,sizeof StartInfo);

				CreateProcess("C:/CX5/bin/terminal.exe", NULL, NULL, NULL, 0, 0, NULL, NULL, &StartInfo, &PInfo);
			}
#endif
			sock_fd = accept(m_sockfd,(struct sockaddr *)&cli_addr,&clilen);
			closesocket(m_sockfd);
			m_sockfd=sock_fd;
			
			send(m_sockfd,">",1,0);
			memset(msg,0,sizeof msg);
			for(i=0;;i++)
			{
				recv(m_sockfd,msg+i,1,0);
				if(msg[i]=='\r' || msg[i]=='\n')
					break;
				send(m_sockfd,msg+i,1,0);
				if(msg[i]==127 || msg[i]==8)
				{
					msg[i]=0;
					if(i>0)
					{
						msg[i-1]=0;
						i-=2;
					}
					else i=-1;
				}
			}

		/** 
	     * Process the command line. 
	     */ 
	{ 
		char *p; 
		int got_host = 0; 

		default_protocol = be_default_protocol; 
		/** Find the appropriate default port. */ 
		{ 
			int i; 
			default_port = 0; /* illegal */

			for (i = 0; backends[i].backend != NULL; i++) 
			{
				if (backends[i].protocol == default_protocol)
				{
					default_port = backends[i].backend->default_port; 
					break; 
				} 
			}
		} 
		cfg.logtype = LGTYP_NONE; 

		do_defaults(NULL, &cfg); 

		p = msg; 

			/** 
			 * Process a couple of command-line options which are more 
			 * easily dealt with before the line is broken up into 
			 * words. These are the soon-to-be-defunct @sessionname and 
			 * the internal-use-only &sharedmemoryhandle, neither of 
			 * which are combined with anything else. 
			 */ 
		while (*p && isspace(*p)) 
			p++; 
		if (*p == '@')
		{
			int i = strlen(p); 
			while (i > 1 && isspace(p[i - 1])) 
				i--; 
			p[i] = '\0'; 
			do_defaults(p + 1, &cfg); 
		} 
		else if (*p == '&')
		{
			// An initial & means we've been given a command line 
			// containing the hex value of a HANDLE for a file 
			// mapping object, which we must then extract as a 
			// config. 

			HANDLE filemap; 
			Config *cp; 
			if (sscanf(p + 1, "%p", &filemap) == 1 && (cp = MapViewOfFile(filemap, FILE_MAP_READ, 0, 0, sizeof(Config))) != NULL)
			{
				cfg = *cp; 
				UnmapViewOfFile(cp); 
				CloseHandle(filemap); 
			}
		} 
		else 
		{ 
		    // Otherwise, break up the command line and deal with 
		    // it sensibly. 

			int argc, i; 
			char **argv; 

			split_into_argv(msg, &argc, &argv, NULL); 

			for (i = 0; i < argc; i++)
			{
				char *p = argv[i]; 
				int ret; 

				ret = cmdline_process_param(p, i+1<argc?argv[i+1]:NULL, 1, &cfg); 
				if (ret == -2)
				{
					cmdline_error("option \"%s\" requires an argument", p); 
				} 
				else if (ret == 2)
				{
					i++;	       /* skip next argument */

				} 
				else if (ret == 1)
				{
					continue;	       /* nothing further needs doing */
				} 
				else if(!strncmp(p,"-terminal:",9))
				{
					sscanf(p,"-terminal:%s",cfg.termtype);
				}
				else if(!strncmp(p,"-geom:",5))
				{
					sscanf(p,"-geom:%dx%d",&cfg.width,&cfg.height);
				}
				else if (!strncmp(p, "-proxy_mode:",12))
				{
					sscanf(p,"-proxy_mode:%d->%s",&local_port,remote_addr);
				}
				else if (!strcmp(p, "-cleanup"))
				{
								/** 
							     * `putty -cleanup'. Remove all registry 
							     * entries associated with PuTTY, and also find 
							     * and delete the random seed file. 
							     */ 
					char *s1, *s2; 
					s1 = dupprintf("This procedure will remove ALL Registry\n" 
					    "entries associated with %s, and will\n" 
					    "also remove the random seed file.\n" 
					    "\n" 
					    "THIS PROCESS WILL DESTROY YOUR SAVED\n" 
					    "SESSIONS. Are you really sure you want\n" 
					    "to continue?", appname); 
					s2 = dupprintf("%s Warning", appname); 
					if (MessageBox(NULL, s1, s2, MB_YESNO | MB_ICONWARNING) == IDYES)
					{
						cleanup_all(); 
					} 
					sfree(s1); 
					sfree(s2); 
					exit(0); 
				} 
				else if (*p != '-')
				{
					char *q = p; 
					if (got_host)
					{
						// If we already have a host name, treat 
						// this argument as a port number. NB we 
						// have to treat this as a saved -P 
						// argument, so that it will be deferred 
						// until it's a good moment to run it. 

						int ret = cmdline_process_param("-P", p, 1, &cfg); 
						assert(ret == 2); 
					} 
					else if (!strncmp(q, "telnet:", 7))
					{
						// If the hostname starts with "telnet:", 
						// set the protocol to Telnet and process 
						// the string as a Telnet URL. 

						char c; 

						q += 7; 
						if (q[0] == '/' && q[1] == '/') 
							q += 2; 
						cfg.protocol = PROT_TELNET; 
						p = q; 
						while (*p && *p != ':' && *p != '/') 
							p++; 
						c = *p; 
						if (*p) 
							*p++ = '\0'; 
						if (c == ':') 
							cfg.port = atoi(p); 
						else 
							cfg.port = -1; 
						strncpy(cfg.host, q, sizeof(cfg.host) - 1); 
						cfg.host[sizeof(cfg.host) - 1] = '\0'; 
						got_host = 1; 
					} 
					else
					{
						// Otherwise, treat this argument as a host 
						// name. 
						while (*p && !isspace(*p)) 
							p++; 
						if (*p) 
							*p++ = '\0'; 
						strncpy(cfg.host, q, sizeof(cfg.host) - 1); 
						cfg.host[sizeof(cfg.host) - 1] = '\0'; 
						got_host = 1; 
					} 
				} 
				else
				{
					cmdline_error("unknown option \"%s\"", p); 
				} 
			} 
		} 

		cmdline_run_saved(&cfg); 

		// Trim leading whitespace off the hostname if it's there. 
		{ 
			int space = strspn(cfg.host, " \t"); 
			memmove(cfg.host, cfg.host+space, 1+strlen(cfg.host)-space); 
		} 

		/// See if host is of the form user@host 
		if (cfg.host[0] != '\0')
		{
			char *atsign = strchr(cfg.host, '@'); 
			// Make sure we're not overflowing the user field 
			if (atsign) 
			{
				if (atsign - cfg.host < sizeof cfg.username)
				{
					strncpy(cfg.username, cfg.host, atsign - cfg.host); 
					cfg.username[atsign - cfg.host] = '\0'; 
				} 
				memmove(cfg.host, atsign + 1, 1 + strlen(atsign + 1)); 
			} 
		} 

		// Trim a colon suffix off the hostname if it's there. 

		cfg.host[strcspn(cfg.host, ":")] = '\0'; 

		// Remove any remaining whitespace from the hostname. 

		{ 
			int p1 = 0, p2 = 0; 
			while (cfg.host[p2] != '\0')
			{
				if (cfg.host[p2] != ' ' && cfg.host[p2] != '\t')
				{
					cfg.host[p1] = cfg.host[p2]; 
					p1++; 
				} 
				p2++; 
			} 
			cfg.host[p1] = '\0'; 
		} 
	} 

	// Select protocol. This is farmed out into a table in a 
	// separate file to enable an ssh-free variant. 

	{ 
		int i; 
		back = NULL; 
		for (i = 0; backends[i].backend != NULL; i++) 
		{
			if (backends[i].protocol == cfg.protocol)
			{
				back = backends[i].backend; 
				break; 
			} 
		}
		if (back == NULL)
		{
			char *str = dupprintf("%s Internal Error", appname); 
			MessageBox(NULL, "Unsupported protocol number found", str, MB_OK | MB_ICONEXCLAMATION); 
			sfree(str); 
			WSACleanup(); 
			return 1; 
		} 
	} 


	/// Check for invalid Port number (i.e. zero) 

	if (cfg.port == 0)
	{
		char *str = dupprintf("%s Internal Error", appname); 
		MessageBox(NULL, "Invalid Port Number", str, MB_OK | MB_ICONEXCLAMATION); 
		sfree(str); 
		WSACleanup(); 
		return 1; 
	} 


			mess = do_select(m_sockfd, 1); 
			if (mess)
				goto SOCK_ERR;
		} 
		error = back->init(NULL, &backhandle, &cfg, cfg.host, cfg.port, &realhost, cfg.tcp_nodelay);
		back->provide_logctx(backhandle, logctx); 
		if (error)
		{
			char *str = dupprintf("%s Error", appname); 
			sprintf(msg, "Unable to open connection to\n" 
			    "%.800s\n" "%s", cfg.host, error); 
			MessageBox(NULL, msg, str, MB_ICONERROR | MB_OK); 
			sfree(str); 
			return 0; 
		} 
		window_name = icon_name = NULL; 
		if (*cfg.wintitle)
		{
			title = cfg.wintitle; 
		}
		else
		{
			sprintf(msg, "%s - %s", realhost, appname); 
			title = msg; 
		} 
		sfree(realhost); 
//		set_title(NULL, title); 
//		set_icon(NULL, title); 
	} 

//	term_provide_resize_fn(term, back->size, backhandle); 
	ldisc = ldisc_create(&cfg, term, back, backhandle, NULL); 

	session_closed = FALSE; 

	lastact = MA_NOTHING; 
	lastbtn = MBT_NOTHING; 
	dbltime = GetDoubleClickTime(); 

//	logfopen(logctx); 

	     // Set the palette up. 
	pal = NULL; 
	logpal = NULL; 
	init_palette(); 

	check_key();

//	term->has_focus = (GetForegroundWindow() == hwnd); 
	UpdateWindow(hwnd); 

	if (GetMessage(&msg, NULL, 0, 0) == 1)
	{
		int timer_id = 0, long_timer = 0; 

		while (msg.message != WM_QUIT)
		{
/*
			if (timer_id && long_timer)
			{
				KillTimer(hwnd, timer_id); 
				long_timer = timer_id = 0; 
			} 
			if (!timer_id) 
				timer_id = SetTimer(hwnd, 1, 20, NULL); 
*/
//			if (!(IsWindow(logbox) && IsDialogMessage(logbox, &msg))) 
				DispatchMessage(&msg); 

			/// Make sure we blink everything that needs it. 
//			term_blink(term, 0); 

			/// Send the paste buffer if there's anything to send 
//			term_paste(term); 

			// If there's nothing new in the queue then we can do everything 
			// we've delayed, reading the socket, writing, and repainting 
			// the window. 

			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
				continue; 

			if(pending_netevent)
			{
				enact_pending_netevent(); 

				if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
					continue; 
			} 

			// Okay there is now nothing to do so we make sure the screen is 
			// completely up to date then tell windows to call us in a little  
			// while. 
/*
			if (timer_id)
			{
				KillTimer(hwnd, timer_id); 
				timer_id = 0; 
			} 
*/

			HideCaret(hwnd); 
/*
			if (GetCapture() != hwnd || (send_raw_mouse && !(cfg.mouse_override && is_shift_pressed())))
				term_out(term); 
*/
//	term_update(term); 
			ShowCaret(hwnd); 

			flash_window(1);	       /// maintain 

/*
			/// The messages seem unreliable; especially if we're being tricky 
			term->has_focus = (GetForegroundWindow() == hwnd); 
			if (term->in_vbell) 
				/// Hmm, term_update didn't want to do an update too soon ...
				timer_id = SetTimer(hwnd, 1, 50, NULL); 
			else if (!term->has_focus) 
				timer_id = SetTimer(hwnd, 1, 500, NULL); 
			else 
				timer_id = SetTimer(hwnd, 1, 100, NULL); 
			long_timer = 1; 
*/
			// There's no point rescanning everything in the message queue 
			// so we do an apparently unnecessary wait here 
			WaitMessage(); 
			if (GetMessage(&msg, NULL, 0, 0) != 1) 
				break; 
		} 
	} 

	cleanup_exit(msg.wParam);	       /* this doesn't return... */

	return msg.wParam;		       /* ... but optimiser doesn't know */

} 

/** 
 * Clean up and exit. 
 */ 
void cleanup_exit(int code) 
{ 
	/** 
	     * Clean up. 
	     */ 
	deinit_fonts(); 
	sfree(logpal); 
	if (pal) 
		DeleteObject(pal); 
	sk_cleanup(); 
	if (wsa_started) 
		WSACleanup(); 

	if (cfg.protocol == PROT_SSH)
	{
		random_save_seed(); 
#ifdef MSCRYPTOAPI 
		crypto_wrapup(); 
#endif 
	} 

	exit(code); 
} 

/** 
 * Set up, or shut down, an AsyncSelect. Called from winnet.c. 
 */ 
char *do_select(SOCKET skt, int startup) 
{ 
	int msg, events; 
	if (startup)
	{
		msg = WM_NETEVENT; 
		events = (FD_CONNECT | FD_READ | FD_WRITE | FD_OOB | FD_CLOSE | FD_ACCEPT); 
	} 
	else
	{
		msg = events = 0; 
	} 
	if (!hwnd) 
		return "do_select(): internal error (hwnd==NULL)"; 
	if (WSAAsyncSelect(skt, hwnd, msg, events) == SOCKET_ERROR)
	{
		switch (WSAGetLastError())
		{
		case WSAENETDOWN: 
			return "Network is down"; 
		default: 
			return "WSAAsyncSelect(): unknown error"; 
		} 
	} 
	return NULL; 
} 

/** 
 * Update the Special Commands submenu. 
 */ 
void update_specials_menu(void *frontend) 
{ 
	HMENU m = GetSystemMenu(hwnd, FALSE); 
	int menu_already_exists = (specials != NULL); 
	int i; 

	specials = back->get_specials(backhandle); 
	if (specials)
	{
		HMENU p = CreateMenu(); 
		for (i = 0; specials[i].name; i++)
		{
			assert(IDM_SPECIAL_MIN + 0x10 * i < IDM_SPECIAL_MAX); 
			if (*specials[i].name) 
				AppendMenu(p, MF_ENABLED, IDM_SPECIAL_MIN + 0x10 * i, 
				specials[i].name); 
			else 
				AppendMenu(p, MF_SEPARATOR, 0, 0); 
		} 
		if (menu_already_exists) 
			DeleteMenu(m, specials_menu_position, MF_BYPOSITION); 
		else 
			InsertMenu(m, specials_menu_position, 
			MF_BYPOSITION | MF_SEPARATOR, 0, 0); 
		InsertMenu(m, specials_menu_position, 
		MF_BYPOSITION | MF_POPUP | MF_ENABLED, 
		(UINT) p, "Special Command"); 
	} 
} 

/** 
 * set or clear the "raw mouse message" mode 
 */ 
void set_raw_mouse_mode(void *frontend, int activate) 
{ 
	activate = activate && !cfg.no_mouse_rep; 
	send_raw_mouse = activate; 
	SetCursor(LoadCursor(NULL, activate ? IDC_ARROW : IDC_IBEAM)); 
} 

/** 
 * Print a message box and close the connection. 
 */ 
void connection_fatal(void *frontend, char *fmt, ...) 
{ 
	va_list ap; 
	char *stuff, morestuff[100]; 

	va_start(ap, fmt); 
	stuff = dupvprintf(fmt, ap); 
	va_end(ap); 
	sprintf(morestuff, "%.70s Fatal Error", appname); 
	MessageBox(hwnd, stuff, morestuff, MB_ICONERROR | MB_OK); 
	sfree(stuff); 

	if (cfg.close_on_exit == FORCE_ON) 
		PostQuitMessage(1); 
	else
	{
		session_closed = TRUE; 
		sprintf(morestuff, "%.70s (inactive)", appname); 
//		set_icon(NULL, morestuff); 
//		set_title(NULL, morestuff); 
	} 
} 

/** 
 * Report an error at the command-line parsing stage. 
 */ 
void cmdline_error(char *fmt, ...) 
{ 
	va_list ap; 
	char *stuff, morestuff[100]; 

	va_start(ap, fmt); 
	stuff = dupvprintf(fmt, ap); 
	va_end(ap); 
	sprintf(morestuff, "%.70s Command Line Error", appname); 
	MessageBox(hwnd, stuff, morestuff, MB_ICONERROR | MB_OK); 
	sfree(stuff); 
	exit(1); 
} 

/** 
 * Actually do the job requested by a WM_NETEVENT 
 */ 
static void enact_pending_netevent(void) 
{ 
	static int reentering = 0; 
	extern int select_result(WPARAM, LPARAM); 
	int ret; 

	if (reentering) 
		return;			       /* don't unpend the pending */


	pending_netevent = FALSE; 

	reentering = 1; 
	ret = select_result(pend_netevent_wParam, pend_netevent_lParam); 
	reentering = 0; 

	if (ret == 0 && !session_closed)
	{
		/** Abnormal exits will already have set session_closed and taken 
			 * appropriate action. */ 
		if (cfg.close_on_exit == FORCE_ON || 
		    cfg.close_on_exit == AUTO) PostQuitMessage(0); 
		else
		{
			char morestuff[100]; 
			session_closed = TRUE; 
			sprintf(morestuff, "%.70s (inactive)", appname); 
//			set_icon(NULL, morestuff); 
//			set_title(NULL, morestuff); 
			MessageBox(hwnd, "Connection closed by remote host", appname, MB_OK | MB_ICONINFORMATION); 
		} 
	} 
} 

/** 
 * Copy the colour palette from the configuration data into defpal. 
 * This is non-trivial because the colour indices are different. 
 */ 
static void cfgtopalette(void) 
{ 
	int i; 
	static const int ww[] =
	{
		6, 7, 8, 9, 10, 11, 12, 13, 
		14, 15, 16, 17, 18, 19, 20, 21, 
		0, 1, 2, 3, 4, 4, 5, 5 
	}; 

	for (i = 0; i < 24; i++)
	{
		int w = ww[i]; 
		defpal[i].rgbtRed = cfg.colours[w][0]; 
		defpal[i].rgbtGreen = cfg.colours[w][1]; 
		defpal[i].rgbtBlue = cfg.colours[w][2]; 
	} 

	/** Override with system colours if appropriate */ 
	if (cfg.system_colour) 
		systopalette(); 
} 

/** 
 * Override bit of defpal with colours from the system. 
 * (NB that this takes a copy the system colours at the time this is called, 
 * so subsequent colour scheme changes don't take effect. To fix that we'd 
 * probably want to be using GetSysColorBrush() and the like.) 
 */ 
static void systopalette(void) 
{ 
	int i; 
	static const struct
	{
		int nIndex; 
		int norm; 
		int bold; 
	} 
	or[] = 
	    {
		{ 
			COLOR_WINDOWTEXT,	16, 17 		}
		, /** Default Foreground */ 
		{ 
			COLOR_WINDOW,		18, 19 		}
		, /** Default Background */ 
		{ 
			COLOR_HIGHLIGHTTEXT,	20, 21 		}
		, /** Cursor Text */ 
		{ 
			COLOR_HIGHLIGHT,	22, 23 		}
		, /** Cursor Colour */ 
	}; 

	for (i = 0; i < (sizeof(or)/sizeof(or[0])); i++) { 
		COLORREF colour = GetSysColor(or[i].nIndex); 
		defpal[or[i].norm].rgbtRed = 
		    defpal[or[i].bold].rgbtRed = GetRValue(colour); 
		defpal[or[i].norm].rgbtGreen = 
		    defpal[or[i].bold].rgbtGreen = GetGValue(colour); 
		defpal[or[i].norm].rgbtBlue = 
		    defpal[or[i].bold].rgbtBlue = GetBValue(colour); 
	} 
} 

/** 
 * Set up the colour palette. 
 */ 
static void init_palette(void) 
{ 
	int i; 
	HDC hdc = GetDC(hwnd); 
	if (hdc)
	{
		if (cfg.try_palette && GetDeviceCaps(hdc, RASTERCAPS) & RC_PALETTE)
		{
			/** 
				     * This is a genuine case where we must use smalloc 
				     * because the snew macros can't cope. 
				     */ 
			logpal = smalloc(sizeof(*logpal) 
			    - sizeof(logpal->palPalEntry) 
			    + NCOLOURS * sizeof(PALETTEENTRY)); 
			logpal->palVersion = 0x300; 
			logpal->palNumEntries = NCOLOURS; 
			for (i = 0; i < NCOLOURS; i++)
			{
				logpal->palPalEntry[i].peRed = defpal[i].rgbtRed; 
				logpal->palPalEntry[i].peGreen = defpal[i].rgbtGreen; 
				logpal->palPalEntry[i].peBlue = defpal[i].rgbtBlue; 
				logpal->palPalEntry[i].peFlags = PC_NOCOLLAPSE; 
			} 
			pal = CreatePalette(logpal); 
			if (pal)
			{
				SelectPalette(hdc, pal, FALSE); 
				RealizePalette(hdc); 
				SelectPalette(hdc, GetStockObject(DEFAULT_PALETTE), FALSE); 
			} 
		} 
		ReleaseDC(hwnd, hdc); 
	} 
	if (pal) 
		for (i = 0; i < NCOLOURS; i++) 
			colours[i] = PALETTERGB(defpal[i].rgbtRed, 
			defpal[i].rgbtGreen, 
			defpal[i].rgbtBlue); 
	else 
		for (i = 0; i < NCOLOURS; i++) 
			colours[i] = RGB(defpal[i].rgbtRed, 
			defpal[i].rgbtGreen, defpal[i].rgbtBlue); 
} 

/** 
 * Initialise all the fonts we will need initially. There may be as many as 
 * three or as few as one.  The other (poentially) twentyone fonts are done 
 * if/when they are needed. 
 * 
 * We also: 
 * 
 * - check the font width and height, correcting our guesses if 
 *   necessary. 
 * 
 * - verify that the bold font is the same width as the ordinary 
 *   one, and engage shadow bolding if not. 
 *  
 * - verify that the underlined font is the same width as the 
 *   ordinary one (manual underlining by means of line drawing can 
 *   be done in a pinch). 
 */ 
static void init_fonts(int pick_width, int pick_height) 
{ 

} 

static void another_font(int fontno) 
{ 

} 

static void deinit_fonts(void) 
{ 

} 


static void set_input_locale(HKL kl) 
{ 
	char lbuf[20]; 

	GetLocaleInfo(LOWORD(kl), LOCALE_IDEFAULTANSICODEPAGE, 
	lbuf, sizeof(lbuf)); 

	kbd_codepage = atoi(lbuf); 
} 

static void click(Mouse_Button b, int x, int y, int shift, int ctrl, int alt) 
{ 

} 

/** 
 * Translate a raw mouse button designation (LEFT, MIDDLE, RIGHT) 
 * into a cooked one (SELECT, EXTEND, PASTE). 
 */ 
static Mouse_Button translate_button(Mouse_Button button) 
{ 

	return 0;			       /* shouldn't happen */

} 

static void show_mouseptr(int show) 
{ 

} 

static int is_alt_pressed(void) 
{ 
	BYTE keystate[256]; 
	int r = GetKeyboardState(keystate); 
	if (!r) 
		return FALSE; 
	if (keystate[VK_MENU] & 0x80) 
		return TRUE; 
	if (keystate[VK_RMENU] & 0x80) 
		return TRUE; 
	return FALSE; 
} 

static int is_shift_pressed(void) 
{ 
	BYTE keystate[256]; 
	int r = GetKeyboardState(keystate); 
	if (!r) 
		return FALSE; 
	if (keystate[VK_SHIFT] & 0x80) 
		return TRUE; 
	return FALSE; 
} 

static int resizing; 

static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) 
{ 
	HDC hdc; 
	static int ignore_clip = FALSE; 
	static int need_backend_resize = FALSE; 
	static int fullscr_on_max = FALSE; 

	switch (message)
	{

	case WM_CREATE: 
		break; 
	case WM_CLOSE: 
		{ 
				DestroyWindow(hwnd); 
		} 
		return 0; 
	case WM_DESTROY: 
//		show_mouseptr(1); 
		PostQuitMessage(0); 
		return 0; 

	case WM_PAINT: 
		{ 
			PAINTSTRUCT p; 
			HideCaret(hwnd); 

			hdc = BeginPaint(hwnd, &p); 
			EndPaint(hwnd, &p); 

			ShowCaret(hwnd); 
		} 
		return 0; 
	case WM_NETEVENT: 
		// Notice we can get multiple netevents, FD_READ, FD_WRITE etc 
			 // but the only one that's likely to try to overload us is FD_READ. 
			 // This means buffering just one is fine. 
			 // 
		if(wParam==m_sockfd)
		{
			int len;
//			term_seen_key_event(term); 
			char buf[512];
			len=recv(m_sockfd,buf,sizeof buf,0);
			if(len<=0)
			{
				return 0;
//				DestroyWindow(hwnd); 
//				exit(0);
			}
			lpage_send(ldisc, CP_ACP, buf, len, 1); 
			return(0);
		}
		if (pending_netevent)			
			enact_pending_netevent(); 

		pending_netevent = TRUE; 
		pend_netevent_wParam = wParam;
		pend_netevent_lParam = lParam; 
		if (WSAGETSELECTEVENT(lParam) != FD_READ) 
			enact_pending_netevent(); 

		time(&last_movement); 
		return 0; 


	default: 
		;
	} 

	return DefWindowProc(hwnd, message, wParam, lParam); 
} 


Context get_ctx(void *frontend) 
{  
	HDC hdc; 
	if (hwnd)
	{
		hdc = GetDC(hwnd); 
		if (hdc && pal) 
			SelectPalette(hdc, pal, FALSE); 
		return hdc; 
	} 
	else 
		return NULL; 
} 

void free_ctx(Context ctx) 
{ 
	SelectPalette(ctx, GetStockObject(DEFAULT_PALETTE), FALSE); 
	ReleaseDC(hwnd, ctx); 
} 

static void real_palette_set(int n, int r, int g, int b) 
{ 
	if (pal)
	{
		logpal->palPalEntry[n].peRed = r; 
		logpal->palPalEntry[n].peGreen = g; 
		logpal->palPalEntry[n].peBlue = b; 
		logpal->palPalEntry[n].peFlags = PC_NOCOLLAPSE; 
		colours[n] = PALETTERGB(r, g, b); 
		SetPaletteEntries(pal, 0, NCOLOURS, logpal->palPalEntry); 
	} 
	else 
		colours[n] = RGB(r, g, b); 
} 

void palette_set(void *frontend, int n, int r, int g, int b) 
{ 
	static const int first[21] =
	{
		0, 2, 4, 6, 8, 10, 12, 14, 
		1, 3, 5, 7, 9, 11, 13, 15, 
		16, 17, 18, 20, 22 
	}; 
	real_palette_set(first[n], r, g, b); 
	if (first[n] >= 18) 
		real_palette_set(first[n] + 1, r, g, b); 
	if (pal)
	{
		HDC hdc = get_ctx(frontend); 
		UnrealizeObject(pal); 
		RealizePalette(hdc); 
		free_ctx(hdc); 
	} 
} 

void palette_reset(void *frontend) 
{ 
	int i; 

	for (i = 0; i < NCOLOURS; i++)
	{
		if (pal)
		{
			logpal->palPalEntry[i].peRed = defpal[i].rgbtRed; 
			logpal->palPalEntry[i].peGreen = defpal[i].rgbtGreen; 
			logpal->palPalEntry[i].peBlue = defpal[i].rgbtBlue; 
			logpal->palPalEntry[i].peFlags = 0; 
			colours[i] = PALETTERGB(defpal[i].rgbtRed, 
			defpal[i].rgbtGreen, 
			defpal[i].rgbtBlue); 
		} 
		else 
			colours[i] = RGB(defpal[i].rgbtRed, 
			defpal[i].rgbtGreen, defpal[i].rgbtBlue); 
	} 

	if (pal)
	{
		HDC hdc; 
		SetPaletteEntries(pal, 0, NCOLOURS, logpal->palPalEntry); 
		hdc = get_ctx(frontend); 
		RealizePalette(hdc); 
		free_ctx(hdc); 
	} 
} 
/*
void write_aclip(void *frontend, char *data, int len, int must_deselect) 
{ 
	HGLOBAL clipdata; 
	void *lock; 

	clipdata = GlobalAlloc(GMEM_DDESHARE | GMEM_MOVEABLE, len + 1); 
	if (!clipdata) 
		return; 
	lock = GlobalLock(clipdata); 
	if (!lock) 
		return; 
	memcpy(lock, data, len); 
	((unsigned char *) lock)[len] = 0; 
	GlobalUnlock(clipdata); 

	if (!must_deselect) 
		SendMessage(hwnd, WM_IGNORE_CLIP, TRUE, 0); 

	if (OpenClipboard(hwnd))
	{
		EmptyClipboard(); 
		SetClipboardData(CF_TEXT, clipdata); 
		CloseClipboard(); 
	} 
	else 
		GlobalFree(clipdata); 

	if (!must_deselect) 
		SendMessage(hwnd, WM_IGNORE_CLIP, FALSE, 0); 
} 


// Note: unlike write_aclip() this will not append a nul. 

void write_clip(void *frontend, wchar_t * data, int len, int must_deselect) 
{ 
	HGLOBAL clipdata, clipdata2, clipdata3; 
	int len2; 
	void *lock, *lock2, *lock3; 

	len2 = WideCharToMultiByte(CP_ACP, 0, data, len, 0, 0, NULL, NULL); 

	clipdata = GlobalAlloc(GMEM_DDESHARE | GMEM_MOVEABLE, 
	len * sizeof(wchar_t)); 
	clipdata2 = GlobalAlloc(GMEM_DDESHARE | GMEM_MOVEABLE, len2); 

	if (!clipdata || !clipdata2)
	{
		if (clipdata) 
			GlobalFree(clipdata); 
		if (clipdata2) 
			GlobalFree(clipdata2); 
		return; 
	} 
	if (!(lock = GlobalLock(clipdata))) 
		return; 
	if (!(lock2 = GlobalLock(clipdata2))) 
		return; 

	memcpy(lock, data, len * sizeof(wchar_t)); 
	WideCharToMultiByte(CP_ACP, 0, data, len, lock2, len2, NULL, NULL); 

	if (cfg.rtf_paste)
	{
		wchar_t unitab[256]; 
		char *rtf = NULL; 
		unsigned char *tdata = (unsigned char *)lock2; 
		wchar_t *udata = (wchar_t *)lock; 
		int rtflen = 0, uindex = 0, tindex = 0; 
		int rtfsize = 0; 
		int multilen, blen, alen, totallen, i; 
		char before[16], after[4]; 

		get_unitab(CP_ACP, unitab, 0); 

		rtfsize = 100 + strlen(cfg.font.name); 
		rtf = snewn(rtfsize, char); 
		sprintf(rtf, "{\\rtf1\\ansi%d{\\fonttbl\\f0\\fmodern %s;}\\f0", 
		GetACP(), cfg.font.name); 
		rtflen = strlen(rtf); 

		//
			 // We want to construct a piece of RTF that specifies the 
			 // same Unicode text. To do this we will read back in 
			 // parallel from the Unicode data in `udata' and the 
			 // non-Unicode data in `tdata'. For each character in 
			 // `tdata' which becomes the right thing in `udata' when 
			 // looked up in `unitab', we just copy straight over from 
			 // tdata. For each one that doesn't, we must WCToMB it 
			 // individually and produce a \u escape sequence. 
			 //  
			 // It would probably be more robust to just bite the bullet 
			 // and WCToMB each individual Unicode character one by one, 
			 // then MBToWC each one back to see if it was an accurate 
			 // translation; but that strikes me as a horrifying number 
			 // of Windows API calls so I want to see if this faster way 
			 // will work. If it screws up badly we can always revert to 
			 // the simple and slow way. 
			 // 
		while (tindex < len2 && uindex < len && 
		    tdata[tindex] && udata[uindex])
		    {
			if (tindex + 1 < len2 && 
			    tdata[tindex] == '\r' && 
			    tdata[tindex+1] == '\n')
			    {
				tindex++; 
				uindex++; 
			} 
			if (unitab[tdata[tindex]] == udata[uindex])
			{
				multilen = 1; 
				before[0] = '\0'; 
				after[0] = '\0'; 
				blen = alen = 0; 
			} 
			else
			{
				multilen = WideCharToMultiByte(CP_ACP, 0, unitab+uindex, 1, 
				NULL, 0, NULL, NULL); 
				if (multilen != 1)
				{
					blen = sprintf(before, "{\\uc%d\\u%d", multilen, 
					udata[uindex]); 
					alen = 1; 
					strcpy(after, "}"); 
				} 
				else
				{
					blen = sprintf(before, "\\u%d", udata[uindex]); 
					alen = 0; 
					after[0] = '\0'; 
				} 
			} 
			assert(tindex + multilen <= len2); 
			totallen = blen + alen; 
			for (i = 0; i < multilen; i++)
			{
				if (tdata[tindex+i] == '\\' || 
				    tdata[tindex+i] == '{' || 
				    tdata[tindex+i] == '}') 
					totallen += 2; 
				else if (tdata[tindex+i] == 0x0D || tdata[tindex+i] == 0x0A) 
					totallen += 6;     // \par\r\n 

				else if (tdata[tindex+i] > 0x7E || tdata[tindex+i] < 0x20) 
					totallen += 4; 
				else 
					totallen++; 
			} 

			if (rtfsize < rtflen + totallen + 3)
			{
				rtfsize = rtflen + totallen + 512; 
				rtf = sresize(rtf, rtfsize, char); 
			} 

			strcpy(rtf + rtflen, before); 
			rtflen += blen; 
			for (i = 0; i < multilen; i++)
			{
				if (tdata[tindex+i] == '\\' || 
				    tdata[tindex+i] == '{' || 
				    tdata[tindex+i] == '}') { 
					rtf[rtflen++] = '\\'; 
					rtf[rtflen++] = tdata[tindex+i]; 
				} 
				else if (tdata[tindex+i] == 0x0D || tdata[tindex+i] == 0x0A)
				{
					rtflen += sprintf(rtf+rtflen, "\\par\r\n"); 
				} 
				else if (tdata[tindex+i] > 0x7E || tdata[tindex+i] < 0x20)
				{
					rtflen += sprintf(rtf+rtflen, "\\'%02x", tdata[tindex+i]); 
				} 
				else
				{
					rtf[rtflen++] = tdata[tindex+i]; 
				} 
			} 
			strcpy(rtf + rtflen, after); 
			rtflen += alen; 

			tindex += multilen; 
			uindex++; 
		} 

		strcpy(rtf + rtflen, "}"); 
		rtflen += 2; 

		clipdata3 = GlobalAlloc(GMEM_DDESHARE | GMEM_MOVEABLE, rtflen); 
		if (clipdata3 && (lock3 = GlobalLock(clipdata3)) != NULL)
		{
			strcpy(lock3, rtf); 
			GlobalUnlock(clipdata3); 
		} 
		sfree(rtf); 
	} 
	else 
		clipdata3 = NULL; 

	GlobalUnlock(clipdata); 
	GlobalUnlock(clipdata2); 

	if (!must_deselect) 
		SendMessage(hwnd, WM_IGNORE_CLIP, TRUE, 0); 

	if (OpenClipboard(hwnd))
	{
		EmptyClipboard(); 
		SetClipboardData(CF_UNICODETEXT, clipdata); 
		SetClipboardData(CF_TEXT, clipdata2); 
		if (clipdata3) 
			SetClipboardData(RegisterClipboardFormat(CF_RTF), clipdata3); 
		CloseClipboard(); 
	} 
	else
	{
		GlobalFree(clipdata); 
		GlobalFree(clipdata2); 
	} 

	if (!must_deselect) 
		SendMessage(hwnd, WM_IGNORE_CLIP, FALSE, 0); 
} 

void get_clip(void *frontend, wchar_t ** p, int *len) 
{ 
	static HGLOBAL clipdata = NULL; 
	static wchar_t *converted = 0; 
	wchar_t *p2; 

	if (converted)
	{
		sfree(converted); 
		converted = 0; 
	} 
	if (!p)
	{
		if (clipdata) 
			GlobalUnlock(clipdata); 
		clipdata = NULL; 
		return; 
	} 
	else if (OpenClipboard(NULL))
	{
		if ((clipdata = GetClipboardData(CF_UNICODETEXT)))
		{
			CloseClipboard(); 
			*p = GlobalLock(clipdata); 
			if (*p)
			{
				for (p2 = *p; *p2; p2++); 
				*len = p2 - *p; 
				return; 
			} 
		} 
		else if ( (clipdata = GetClipboardData(CF_TEXT)) )
		{
			char *s; 
			int i; 
			CloseClipboard(); 
			s = GlobalLock(clipdata); 
			i = MultiByteToWideChar(CP_ACP, 0, s, strlen(s) + 1, 0, 0); 
			*p = converted = snewn(i, wchar_t); 
			MultiByteToWideChar(CP_ACP, 0, s, strlen(s) + 1, converted, i); 
			*len = i - 1; 
			return; 
		} 
		else 
			CloseClipboard(); 
	} 

	*p = NULL; 
	*len = 0; 
} 

*/
/** 
 * Print a message box and perform a fatal exit. 
 */ 
void fatalbox(char *fmt, ...) 
{ 
	va_list ap; 
	char *stuff, morestuff[100]; 

	va_start(ap, fmt); 
	stuff = dupvprintf(fmt, ap); 
	va_end(ap); 
	sprintf(morestuff, "%.70s Fatal Error", appname); 
	MessageBox(hwnd, stuff, morestuff, MB_ICONERROR | MB_OK); 
	sfree(stuff); 
	cleanup_exit(1); 
} 

/** 
 * Print a modal (Really Bad) message box and perform a fatal exit. 
 */ 
void modalfatalbox(char *fmt, ...) 
{ 
	va_list ap; 
	char *stuff, morestuff[100]; 

	va_start(ap, fmt); 
	stuff = dupvprintf(fmt, ap); 
	va_end(ap); 
	sprintf(morestuff, "%.70s Fatal Error", appname); 
	MessageBox(hwnd, stuff, morestuff, 
	MB_SYSTEMMODAL | MB_ICONERROR | MB_OK); 
	sfree(stuff); 
	cleanup_exit(1); 
} 

/** 
 * Manage window caption / taskbar flashing, if enabled. 
 * 0 = stop, 1 = maintain, 2 = start 
 */ 
static void flash_window(int mode) 
{ 
	static long last_flash = 0; 
	static int flashing = 0; 
	if ((mode == 0) || (cfg.beep_ind == B_IND_DISABLED))
	{
		/** stop */ 
		if (flashing)
		{
			FlashWindow(hwnd, FALSE); 
			flashing = 0; 
		} 

	} 
	else if (mode == 2)
	{
		/** start */ 
		if (!flashing)
		{
			last_flash = GetTickCount(); 
			flashing = 1; 
			FlashWindow(hwnd, TRUE); 
		} 

	} 
	else if ((mode == 1) && (cfg.beep_ind == B_IND_FLASH))
	{
		/** maintain */ 
		if (flashing)
		{
			long now = GetTickCount(); 
			long fdiff = now - last_flash; 
			if (fdiff < 0 || fdiff > 450)
			{
				last_flash = now; 
				FlashWindow(hwnd, TRUE);	/* toggle */

			} 
		} 
	} 
} 

/** 
 * Beep. 

void beep(void *frontend, int mode) 
{ 
	if (mode == BELL_DEFAULT)
	{
		//
			 // For MessageBeep style bells, we want to be careful of 
			 // timing, because they don't have the nice property of 
			 // PlaySound bells that each one cancels the previous 
			 // active one. So we limit the rate to one per 50ms or so. 
			 // 
		static long lastbeep = 0; 
		long beepdiff; 

		beepdiff = GetTickCount() - lastbeep; 
		if (beepdiff >= 0 && beepdiff < 50) 
			return; 
		MessageBeep(MB_OK); 
		// 
			 // The above MessageBeep call takes time, so we record the 
			 // time _after_ it finishes rather than before it starts. 
			 /// 
		lastbeep = GetTickCount(); 
	} 
	else if (mode == BELL_WAVEFILE)
	{
		if (!PlaySound(cfg.bell_wavefile.path, NULL, 
		SND_ASYNC | SND_FILENAME))
		{
			char buf[sizeof(cfg.bell_wavefile) + 80]; 
			char otherbuf[100]; 
			sprintf(buf, "Unable to play sound file\n%s\n" 
			    "Using default sound instead", cfg.bell_wavefile); 
			sprintf(otherbuf, "%.70s Sound Error", appname); 
			MessageBox(hwnd, buf, otherbuf, 
			MB_OK | MB_ICONEXCLAMATION); 
			cfg.beep = BELL_DEFAULT; 
		} 
	} 
	else if (mode == BELL_PCSPEAKER)
	{
		static long lastbeep = 0; 
		long beepdiff; 

		beepdiff = GetTickCount() - lastbeep; 
		if (beepdiff >= 0 && beepdiff < 50) 
			return; 

		//
			 // We must beep in different ways depending on whether this 
			 // is a 95-series or NT-series OS. 
			 /// 
		if(osVersion.dwPlatformId == VER_PLATFORM_WIN32_NT) 
			Beep(800, 100); 
		else 
			MessageBeep(-1); 
		lastbeep = GetTickCount(); 
	} 
	// Otherwise, either visual bell or disabled; do nothing here 
	if (!term->has_focus)
	{
		flash_window(2);	       // start 

	} 
} 

/** 
 * Minimise or restore the window in response to a server-side 
 * request. 
 */ 
void set_iconic(void *frontend, int iconic) 
{ 
	if (IsIconic(hwnd))
	{
		if (!iconic) 
			ShowWindow(hwnd, SW_RESTORE); 
	} 
	else
	{
		if (iconic) 
			ShowWindow(hwnd, SW_MINIMIZE); 
	} 
} 

/** 
 * Move the window in response to a server-side request. 
 */ 
void move_window(void *frontend, int x, int y) 
{ 
	if (cfg.resize_action == RESIZE_DISABLED ||  cfg.resize_action == RESIZE_FONT || IsZoomed(hwnd)) 
		return; 

	SetWindowPos(hwnd, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER); 
} 

/** 
 * Move the window to the top or bottom of the z-order in response 
 * to a server-side request. 
 */ 
void set_zorder(void *frontend, int top) 
{ 
	if (cfg.alwaysontop) 
		return;			       /* ignore */

	SetWindowPos(hwnd, top ? HWND_TOP : HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); 
} 

/** 
 * Refresh the window in response to a server-side request. 
 */ 
void refresh_window(void *frontend) 
{ 
	InvalidateRect(hwnd, NULL, TRUE); 
} 

/** 
 * Maximise or restore the window in response to a server-side 
 * request. 
 */ 
void set_zoomed(void *frontend, int zoomed) 
{ 
	if (IsZoomed(hwnd))
	{
		if (!zoomed) 
			ShowWindow(hwnd, SW_RESTORE); 
	} 
	else
	{
		if (zoomed) 
			ShowWindow(hwnd, SW_MAXIMIZE); 
	} 
} 

/** 
 * Report whether the window is iconic, for terminal reports. 
 */ 
int is_iconic(void *frontend) 
{ 
	return IsIconic(hwnd); 
} 

/** 
 * Report the window's position, for terminal reports. 
 */ 
void get_window_pos(void *frontend, int *x, int *y) 
{ 
	RECT r; 
	GetWindowRect(hwnd, &r); 
	*x = r.left; 
	*y = r.top; 
} 

/** 
 * Report the window's pixel size, for terminal reports. 
 */ 
void get_window_pixels(void *frontend, int *x, int *y) 
{ 
	RECT r; 
	GetWindowRect(hwnd, &r); 
	*x = r.right - r.left; 
	*y = r.bottom - r.top; 
} 

/** 
 * Return the window or icon title. 
 */ 
char *get_window_title(void *frontend, int icon) 
{ 
	return icon ? icon_name : window_name; 
} 

/** 
 * See if we're in full-screen mode. 
 */ 
int is_full_screen() 
{ 
	if (!IsZoomed(hwnd)) 
		return FALSE; 
	if (GetWindowLong(hwnd, GWL_STYLE) & WS_CAPTION) 
		return FALSE; 
	return TRUE; 
} 

/** Get the rect/size of a full screen window using the nearest available 
 * monitor in multimon systems; default to something sensible if only 
 * one monitor is present. */ 
static int get_fullscreen_rect(RECT * ss) 
{ 
#ifdef MONITOR_DEFAULTTONEAREST 
	HMONITOR mon; 
	MONITORINFO mi; 
	mon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST); 
	mi.cbSize = sizeof(mi); 
	GetMonitorInfo(mon, &mi); 

	/** structure copy */ 
	*ss = mi.rcMonitor; 
	return TRUE; 
#else 
	/** could also use code like this: 
		ss->left = ss->top = 0; 
		ss->right = GetSystemMetrics(SM_CXSCREEN); 
		ss->bottom = GetSystemMetrics(SM_CYSCREEN); 
	*/  
	return GetClientRect(GetDesktopWindow(), ss); 
#endif 
} 


/** 
 * Go full-screen. This should only be called when we are already 
 * maximised. 
 */ 
void make_full_screen() 
{ 
	DWORD style; 
	RECT ss; 

	assert(IsZoomed(hwnd)); 

	if (is_full_screen()) 
		return; 

	// Remove the window furniture. 
	style = GetWindowLong(hwnd, GWL_STYLE); 
	style &= ~(WS_CAPTION | WS_BORDER | WS_THICKFRAME); 
	if (cfg.scrollbar_in_fullscreen) 
		style |= WS_VSCROLL; 
	else 
		style &= ~WS_VSCROLL; 
	SetWindowLong(hwnd, GWL_STYLE, style); 

	// Resize ourselves to exactly cover the nearest monitor. 
	get_fullscreen_rect(&ss); 
	SetWindowPos(hwnd, HWND_TOP, ss.left, ss.top, 
	ss.right - ss.left, 
	ss.bottom - ss.top, 
	SWP_FRAMECHANGED); 

	// Tick the menu item in the System menu. 
	CheckMenuItem(GetSystemMenu(hwnd, FALSE), IDM_FULLSCREEN, 
	MF_CHECKED); 
} 

/** 
 * Clear the full-screen attributes. 
 */ 
void clear_full_screen() 
{ 
	DWORD oldstyle, style; 

	/** Reinstate the window furniture. */ 
	style = oldstyle = GetWindowLong(hwnd, GWL_STYLE); 
	style |= WS_CAPTION | WS_BORDER; 
	if (cfg.resize_action == RESIZE_DISABLED) 
		style &= ~WS_THICKFRAME; 
	else 
		style |= WS_THICKFRAME; 
	if (cfg.scrollbar) 
		style |= WS_VSCROLL; 
	else 
		style &= ~WS_VSCROLL; 
	if (style != oldstyle)
	{
		SetWindowLong(hwnd, GWL_STYLE, style); 
		SetWindowPos(hwnd, NULL, 0, 0, 0, 0, 
		SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | 
		    SWP_FRAMECHANGED); 
	} 

	/** Untick the menu item in the System menu. */ 
	CheckMenuItem(GetSystemMenu(hwnd, FALSE), IDM_FULLSCREEN, 
	MF_UNCHECKED); 
} 

/** 
 * Toggle full-screen mode. 
 */ 
void flip_full_screen() 
{ 
	if (is_full_screen())
	{
		ShowWindow(hwnd, SW_RESTORE); 
	} 
	else if (IsZoomed(hwnd))
	{
		make_full_screen(); 
	} 
	else
	{
		SendMessage(hwnd, WM_FULLSCR_ON_MAX, 0, 0); 
		ShowWindow(hwnd, SW_MAXIMIZE); 
	} 
} 

void frontend_keypress(void *handle) 
{ 
	/** 
	     * Keypress termination in non-Close-On-Exit mode is not 
	     * currently supported in PuTTY proper, because the window 
	     * always has a perfectly good Close button anyway. So we do 
	     * nothing here. 
	     */ 
	return; 
} 

int from_backend(void *frontend, int is_stderr, const char *data, int len) 
{
	if(m_sockfd!=0)
		send(m_sockfd,data,len,0);
	return(len);
//	return term_data(term, is_stderr, data, len); 
} 

void agent_schedule_callback(void (*callback)(void *, void *, int), 
void *callback_ctx, void *data, int len) 
{ 
	struct agent_callback *c = snew(struct agent_callback); 
	c->callback = callback; 
	c->callback_ctx = callback_ctx; 
	c->data = data; 
	c->len = len; 
	PostMessage(hwnd, WM_AGENT_CALLBACK, 0, (LPARAM)c); 
} 

void verify_ssh_host_key(void *frontend, char *host, int port, char *keytype, char *keystr, char *fingerprint)
{
    int ret;

    static const char absentmsg[] =
	"The server's host key is not cached in the registry. You\n"
	"have no guarantee that the server is the computer you\n"
	"think it is.\n"
	"The server's %s key fingerprint is:\n"
	"%s\n"
	"If you trust this host, hit Yes to add the key to\n"
	"%s's cache and carry on connecting.\n"
	"If you want to carry on connecting just once, without\n"
	"adding the key to the cache, hit No.\n"
	"If you do not trust this host, hit Cancel to abandon the\n"
	"connection.\n";

    static const char wrongmsg[] =
	"WARNING - POTENTIAL SECURITY BREACH!\n"
	"\n"
	"The server's host key does not match the one %s has\n"
	"cached in the registry. This means that either the\n"
	"server administrator has changed the host key, or you\n"
	"have actually connected to another computer pretending\n"
	"to be the server.\n"
	"The new %s key fingerprint is:\n"
	"%s\n"
	"If you were expecting this change and trust the new key,\n"
	"hit Yes to update %s's cache and continue connecting.\n"
	"If you want to carry on connecting but without updating\n"
	"the cache, hit No.\n"
	"If you want to abandon the connection completely, hit\n"
	"Cancel. Hitting Cancel is the ONLY guaranteed safe\n" "choice.\n";

    static const char mbtitle[] = "%s Security Alert";

    /*
     * Verify the key against the registry.
     */
    ret = verify_host_key(host, port, keytype, keystr);

    if (ret == 0)		       /* success - key matched OK */
	return;
    if (ret == 2) {		       /* key was different */
	int mbret;
	char *message, *title;
	message = dupprintf(wrongmsg, appname, keytype, fingerprint, appname);
	title = dupprintf(mbtitle, appname);
	mbret = MessageBox(NULL, message, title,
			   MB_ICONWARNING | MB_YESNOCANCEL);
	sfree(message);
	sfree(title);
	if (mbret == IDYES)
	    store_host_key(host, port, keytype, keystr);
	if (mbret == IDCANCEL)
	    cleanup_exit(0);
    }
    if (ret == 1) {		       /* key was absent */
	int mbret;
	char *message, *title;
	message = dupprintf(absentmsg, keytype, fingerprint, appname);
	title = dupprintf(mbtitle, appname);
	mbret = MessageBox(NULL, message, title,
			   MB_ICONWARNING | MB_YESNOCANCEL);
	sfree(message);
	sfree(title);
	if (mbret == IDYES)
	    store_host_key(host, port, keytype, keystr);
	if (mbret == IDCANCEL)
	    cleanup_exit(0);
    }
}