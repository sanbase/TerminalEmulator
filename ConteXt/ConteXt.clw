; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=Setting_Dialog
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "ConteXt.h"
LastPage=0

ClassCount=7
Class1=CConteXtApp
Class2=CConteXtDoc
Class3=CConteXtView
Class4=CMainFrame

ResourceCount=4
Resource1=IDD_ABOUTBOX
Class5=CAboutDlg
Resource2=IDD_DIALOG1
Class6=Setting_Dialog
Resource3=IDR_MAINFRAME
Class7=Registration
Resource4=IDD_DIALOG2

[CLS:CConteXtApp]
Type=0
HeaderFile=ConteXt.h
ImplementationFile=ConteXt.cpp
Filter=N

[CLS:CConteXtDoc]
Type=0
HeaderFile=ConteXtDoc.h
ImplementationFile=ConteXtDoc.cpp
Filter=N

[CLS:CConteXtView]
Type=0
HeaderFile=ConteXtView.h
ImplementationFile=ConteXtView.cpp
Filter=C
LastObject=CConteXtView
BaseClass=CTerminalView
VirtualFilter=VWC


[CLS:CMainFrame]
Type=0
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
Filter=T
BaseClass=CFrameWnd
VirtualFilter=fWC
LastObject=ID_CONNECT




[CLS:CAboutDlg]
Type=0
HeaderFile=ConteXt.cpp
ImplementationFile=ConteXt.cpp
Filter=D
LastObject=CAboutDlg

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[MNU:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_CONNECT
Command2=ID_SETUP
Command3=ID_APP_ABOUT
CommandCount=3

[DLG:IDD_DIALOG1]
Type=1
Class=Setting_Dialog
ControlCount=6
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308352
Control4=IDC_EDIT1,edit,1350631552
Control5=IDC_COMBO1,combobox,1478557954
Control6=IDC_STATIC,static,1342308352

[CLS:Setting_Dialog]
Type=0
HeaderFile=Setting_Dialog.h
ImplementationFile=Setting_Dialog.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=Setting_Dialog

[DLG:IDD_DIALOG2]
Type=1
Class=Registration
ControlCount=7
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308352
Control4=IDC_EDIT1,edit,1350631552
Control5=IDC_BUTTON1,button,1342242816
Control6=IDC_EDIT2,edit,1342244992
Control7=IDC_STATIC,static,1342308352

[CLS:Registration]
Type=0
HeaderFile=Registration.h
ImplementationFile=Registration.cpp
BaseClass=CDialog
Filter=D
LastObject=ID_APP_ABOUT

