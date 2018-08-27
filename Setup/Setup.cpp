// Setup.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include <stdio.h>
#include <process.h> 
#include <io.h>
#include <direct.h>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define IDC_RB_SUM 1
#define IDC_RB_DIF 2
#define IDC_RB_MUL 3
#define IDC_RB_DIV 4
#define IDC_PB_RUN 5
#define IDC_EB_OP1 6
#define IDC_EB_OP2 7
#define IDC_EB_RES 8

char *mess_dial;

#include <windows.h>
#include <shlobj.h>
//#include <shlwapi.h>
//#include <shobjidl.h>
//#include <shlguid.h>

HRESULT CreateLink(LPCSTR lpszPathObj, LPCSTR lpszPathLink, LPCSTR lpszDesc) 
{ 
    HRESULT hres; 
    IShellLink *psl ; 
    CoInitialize (NULL);


   // Get a pointer to the IShellLink interface. 
    hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, 
                            IID_IShellLink, (LPVOID*)&psl); 
    if (SUCCEEDED(hres)) 
    { 
        IPersistFile* ppf; 
 
       // Set the path to the shortcut target and add the description. 
        hres = psl->SetPath(lpszPathObj); 
        hres = psl->SetDescription(lpszDesc); 

        // Query IShellLink for the IPersistFile interface for saving the 
        // shortcut in persistent storage. 

        hres = psl->QueryInterface(IID_IPersistFile, (LPVOID*)&ppf);  

       if (SUCCEEDED(hres)) 
        { 
            WCHAR wsz[MAX_PATH]; 
 
           // Ensure that the string is Unicode. 
           hres =  MultiByteToWideChar(CP_ACP, 0, lpszPathLink, -1, wsz, MAX_PATH); 

            // TODO: Check return value from MultiByteWideChar to ensure 
            //         success.

            // Save the link by calling IPersistFile::Save. 
            hres = ppf->Save(wsz, true); 
            ppf->Release(); 
        } 
        psl->Release(); 
    } 
    
    return hres; 
}

/* This function is called by the Windows function DispatchMessage( ) */
LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
static HWND hPB_Run = NULL;

  switch (message)                  /* handle the messages */
  {
    case WM_CREATE:
		CreateWindow( "STATIC"
                            , mess_dial
                            , WS_VISIBLE | WS_CHILD | WS_GROUP | ES_NUMBER 
                            , 10, 10
                            , 580, 17
                            , hwnd
                            , (HMENU) IDC_EB_OP1
                            , NULL
                            , NULL);
		hPB_Run = CreateWindow( "BUTTON"
                            , "OK"
                            , WS_VISIBLE | WS_CHILD |WS_GROUP
                            , 30, 40
                            , 70, 17
                            , hwnd
                            , (HMENU) IDC_PB_RUN
                            , NULL
                            , NULL);


      break;
    case WM_COMMAND:
      if (LOWORD(wParam) == IDC_PB_RUN)
	  {
		  int i=0;
		  exit(0);
	  }
      break;
    case WM_DESTROY:
      PostQuitMessage(0);      /* send a WM_QUIT to the message queue */
      break;
    default:                   /* for messages that we don't deal with */
      return DefWindowProc(hwnd, message, wParam, lParam);
  }
  return 0;
}

/*
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
*/
char szClassName[ ] = "WindowsApp";

int fcopy(char *a,char *b) 
{ 
	int fd,df,size; 
	char buf[1024]; 
	int length=0; 
 
	if((fd=open(b,O_RDONLY|O_BINARY))<0) 
		return(-1); 

	if((df=_open(a,_O_RDWR|_O_BINARY|_O_TRUNC|_O_CREAT,_S_IREAD|_S_IWRITE))<0)
	{ 
		close(fd); 
		return(-2); 
	} 
	while((size=read(fd,buf,1024))>0) 
	{ 
		write(df,buf,size); 
		length+=size; 
	} 
	close(fd); 
	close(df); 
	return(0); 
} 

int WINAPI WinMain(HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nFunsterStil)

{
    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof(WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon(hThisInstance, MAKEINTRESOURCE(500));
    wincl.hIconSm = LoadIcon(hThisInstance, MAKEINTRESOURCE(500));
    wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL; /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */

    /* Use light-gray as the background of the window */
    wincl.hbrBackground = (HBRUSH) GetStockObject(LTGRAY_BRUSH);

    /* Register the window class, if fail quit the program */
    if(!RegisterClassEx(&wincl)) 
		return 0;

	char path[256];

	int ret = GetEnvironmentVariable("USERNAME", path, sizeof(path)); 

//	ret=GetEnvironmentVariable("HOMEPATH", path, sizeof(path)); 

	mess_dial="Setup is completed. The folder C:\\Program Files\\UnixSpace has been created.";
	
	if(!access("UnixSpace.exe",0))
	{
		if(!access("termcap",0))
		{
			if(_access("C:\\Program Files\\UnixSpace",0)==-1)
			{
				if(_mkdir("C:\\Program Files\\UnixSpace"))
				{
					mess_dial="Can't create folder C:\\Program Files\\UnixSpace";
					goto END;
				}
			}
			if(_access("C:\\Program Files\\UnixSpace\\etc",0)==-1)
			{
				if(_mkdir("C:\\Program Files\\UnixSpace\\etc"))
				{
					mess_dial="Can't create folder C:\\Program Files\\UnixSpace\\etc";
					goto END;
				}
			}
			if(_access("C:\\Program Files\\UnixSpace\\bin",0)==-1)
			{
				if(_mkdir("C:\\Program Files\\UnixSpace\\bin"))
				{
					mess_dial="Can't create folder C:\\Program Files\\UnixSpace\\bin";
					goto END;
				}
			}
			if(fcopy("C:\\Program Files\\UnixSpace\\bin\\UnixSpace.exe","UnixSpace.exe")<0)
			{
				mess_dial="Can't create file C:\\Program Files\\UnixSpace\\bin\\UnixSpace.exe";
				goto END;
			}
			if(fcopy("C:\\Program Files\\UnixSpace\\bin\\SSHProxy.exe","SSHProxy.exe")<0)
			{
				mess_dial="Can't create file C:\\Program Files\\UnixSpace\\bin\\SSHProxy.exe";
				goto END;
			}
			if(fcopy("C:\\Program Files\\UnixSpace\\etc\\termcap","termcap")<0)
			{
				mess_dial="Can't create file C:\\Program Files\\UnixSpace\\etc\\termcap";
				goto END;
			}
			if(ret)
			{
				char name[256];
				sprintf(name,"C:\\Documents and Settings\\%s\\Desktop\\Terminal.lnk",path);
				CreateLink("C:\\Program Files\\UnixSpace\\bin\\UnixSpace.exe",name,"Shortcut to Terminal");
				sprintf(name,"C:\\Documents and Settings\\%s\\Start Menu\\Programs\\Terminal.lnk",path);
				CreateLink("C:\\Program Files\\UnixSpace\\bin\\UnixSpace.exe",name,"Shortcut to Terminal");
			}
		}
		else	mess_dial="The file Termcap not found.";
	}
	else	mess_dial="The file UnixSpace.exe not found.";
END:
    /* The class is registered, let's create the program*/
    hwnd = CreateWindow(
           szClassName,         /* Classname */
           "Setup",				/* Title Text */
           WS_OVERLAPPEDWINDOW & ~(WS_THICKFRAME | WS_MAXIMIZEBOX), /* default window */
           CW_USEDEFAULT,       /* Windows decides the position */
           CW_USEDEFAULT,       /* where the window ends up on the screen */
           600,                 /* The programs width */
           100,                 /* and height in pixels */
           HWND_DESKTOP,        /* The window is a child-window to desktop */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );

    /* Make the window visible on the screen */
    ShowWindow(hwnd, nFunsterStil);
    /* Run the message loop. It will run until GetMessage( ) returns 0 */
    while(GetMessage(&messages, NULL, 0, 0))
    {
           /* Translate virtual-key messages into character messages */
           TranslateMessage(&messages);
           /* Send message to WindowProcedure */
           DispatchMessage(&messages);
    }

    /* The program return-value is 0 - The value that PostQuitMessage( ) gave */
    return messages.wParam;
	return 0;
}


