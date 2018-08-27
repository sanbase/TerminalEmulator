; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=FrameGL
LastTemplate=CFrameWnd
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "terminal.h"
LastPage=0

ClassCount=10
Class1=CTerminalApp
Class2=CTerminalDoc
Class3=CTerminalView
Class4=CMainFrame

ResourceCount=4
Resource1=IDD_DIALOG2
Class5=CAboutDlg
Resource2=IDD_ABOUTBOX
Class6=Host_Dialog
Class7=TelnetSocket
Class8=Registration
Class9=Setting
Resource3=IDR_MAINFRAME
Class10=FrameGL
Resource4=IDD_DIALOG3

[CLS:CTerminalApp]
Type=0
HeaderFile=terminal.h
ImplementationFile=terminal.cpp
Filter=N
LastObject=CTerminalApp
BaseClass=CWinApp
VirtualFilter=AC

[CLS:CTerminalDoc]
Type=0
HeaderFile=terminalDoc.h
ImplementationFile=terminalDoc.cpp
Filter=N
LastObject=CTerminalDoc

[CLS:CTerminalView]
Type=0
HeaderFile=terminalView.h
ImplementationFile=terminalView.cpp
Filter=C
BaseClass=CView
VirtualFilter=VWC
LastObject=CTerminalView


[CLS:CMainFrame]
Type=0
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
Filter=T
LastObject=CMainFrame




[CLS:CAboutDlg]
Type=0
HeaderFile=terminal.cpp
ImplementationFile=terminal.cpp
Filter=D
LastObject=CAboutDlg

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=5
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889
Control5=IDC_STATIC,static,1342308352

[MNU:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_HOSTSETTING
Command2=ID_CONNECT
Command3=ID_APP_ABOUT
Command4=ID_SCREEN1
Command5=ID_SCREEN2
Command6=ID_SCREEN3
Command7=ID_SCREEN4
CommandCount=7

[CLS:Host_Dialog]
Type=0
HeaderFile=Host_Dialog.h
ImplementationFile=Host_Dialog.cpp
BaseClass=CDialog
Filter=D
LastObject=IDC_HOST1
VirtualFilter=dWC

[CLS:TelnetSocket]
Type=0
HeaderFile=TelnetSocket.h
ImplementationFile=TelnetSocket.cpp
BaseClass=CAsyncSocket
Filter=N
LastObject=ID_APP_ABOUT
VirtualFilter=q

[CLS:Registration]
Type=0
HeaderFile=Registration.h
ImplementationFile=Registration.cpp
BaseClass=CDialog
Filter=D
LastObject=Registration
VirtualFilter=dWC

[DLG:IDD_DIALOG3]
Type=1
Class=Setting
ControlCount=46
Control1=IDOK2,button,1342242817
Control2=IDCANCEL2,button,1342242816
Control3=IDC_HOST1,edit,1350631552
Control4=IDC_TERMINALLIST1,combobox,1344339970
Control5=IDC_PROTOCOL1,combobox,1344339970
Control6=IDC_LINES1,edit,1350631552
Control7=IDC_STATIC,static,1342308352
Control8=IDC_STATIC,static,1342308352
Control9=IDC_STATIC,static,1342308352
Control10=IDC_STATIC,static,1342308352
Control11=IDC_STATIC,static,1342308352
Control12=IDC_HOST2,edit,1350631552
Control13=IDC_TERMINALLIST2,combobox,1344339970
Control14=IDC_PROTOCOL2,combobox,1344339970
Control15=IDC_LINES2,edit,1350631552
Control16=IDC_STATIC,static,1342308352
Control17=IDC_HOST3,edit,1350631552
Control18=IDC_TERMINALLIST3,combobox,1344339970
Control19=IDC_PROTOCOL3,combobox,1344339970
Control20=IDC_LINES3,edit,1350631552
Control21=IDC_STATIC,static,1342308352
Control22=IDC_HOST4,edit,1350631552
Control23=IDC_TERMINALLIST4,combobox,1344339970
Control24=IDC_PROTOCOL4,combobox,1344339970
Control25=IDC_LINES4,edit,1350631552
Control26=IDC_STATIC,static,1342308352
Control27=IDC_PORT1,edit,1350631552
Control28=IDC_PORT2,edit,1350631552
Control29=IDC_PORT3,edit,1350631552
Control30=IDC_PORT4,edit,1350631552
Control31=IDC_STATIC,static,1342308352
Control32=IDC_STATIC,button,1342177287
Control33=IDC_PROXY_HOST,edit,1350631552
Control34=IDC_PROXY_PORT,edit,1350631552
Control35=IDC_PROXY,combobox,1344339970
Control36=IDC_STATIC,button,1342178055
Control37=IDC_RADIO1,button,1342308361
Control38=IDC_RADIO2,button,1342177289
Control39=IDC_RADIO3,button,1342177289
Control40=IDC_RADIO4,button,1342177289
Control41=IDC_CONNECT,button,1342242816
Control42=IDC_CHARSET1,combobox,1344340226
Control43=IDC_CHARSET2,combobox,1344340226
Control44=IDC_CHARSET3,combobox,1344340226
Control45=IDC_CHARSET4,combobox,1344340226
Control46=IDC_STATIC,static,1342308352

[CLS:Setting]
Type=0
HeaderFile=Setting.h
ImplementationFile=Setting.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=IDC_CHARSET1

[DLG:IDD_DIALOG2]
Type=1
Class=?
ControlCount=7
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308352
Control4=IDC_EDIT1,edit,1350631552
Control5=IDC_BUTTON1,button,1342242816
Control6=IDC_EDIT2,edit,1342244992
Control7=IDC_STATIC,static,1342308352

[CLS:FrameGL]
Type=0
HeaderFile=FrameGL.h
ImplementationFile=FrameGL.cpp
BaseClass=CFrameWnd
Filter=T
VirtualFilter=fWC
LastObject=FrameGL

