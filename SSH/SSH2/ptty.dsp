# Microsoft Developer Studio Project File - Name="ptty" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=ptty - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ptty.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ptty.mak" CFG="ptty - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ptty - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "ptty - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ptty - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib comctl32.lib Imm32.lib winmm.lib /nologo /subsystem:windows /machine:I386 /out:"Release/SSHProxy.exe"

!ELSEIF  "$(CFG)" == "ptty - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib comctl32.lib Imm32.lib winmm.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "ptty - Win32 Release"
# Name "ptty - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\BE_ALL.C
# End Source File
# Begin Source File

SOURCE=..\CMDLINE.C
# End Source File
# Begin Source File

SOURCE=..\ldisc.c
# End Source File
# Begin Source File

SOURCE=..\ldiscucs.c
# End Source File
# Begin Source File

SOURCE=..\logging.c
# End Source File
# Begin Source File

SOURCE=..\MISC.C
# End Source File
# Begin Source File

SOURCE=..\NOISE.C
# End Source File
# Begin Source File

SOURCE=..\PAGEANTC.C
# End Source File
# Begin Source File

SOURCE=..\PORTFWD.C
# End Source File
# Begin Source File

SOURCE=..\PPROXY.C
# End Source File
# Begin Source File

SOURCE=..\PRINTING.C
# End Source File
# Begin Source File

SOURCE=..\PROXY.C
# End Source File
# Begin Source File

SOURCE=..\RAW.C
# End Source File
# Begin Source File

SOURCE=..\RLOGIN.C
# End Source File
# Begin Source File

SOURCE=..\SETTINGS.C
# End Source File
# Begin Source File

SOURCE=..\SIZETIP.C
# End Source File
# Begin Source File

SOURCE=..\SSH.C

!IF  "$(CFG)" == "ptty - Win32 Release"

!ELSEIF  "$(CFG)" == "ptty - Win32 Debug"

# ADD CPP /Zi

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\SSHAES.C
# End Source File
# Begin Source File

SOURCE=..\SSHBLOWF.C
# End Source File
# Begin Source File

SOURCE=..\SSHBN.C
# End Source File
# Begin Source File

SOURCE=..\SSHCRC.C
# End Source File
# Begin Source File

SOURCE=..\SSHCRCDA.C
# End Source File
# Begin Source File

SOURCE=..\SSHDES.C
# End Source File
# Begin Source File

SOURCE=..\SSHDH.C
# End Source File
# Begin Source File

SOURCE=..\SSHDSS.C
# End Source File
# Begin Source File

SOURCE=..\SSHDSSG.C
# End Source File
# Begin Source File

SOURCE=..\SSHMD5.C
# End Source File
# Begin Source File

SOURCE=..\SSHPRIME.C
# End Source File
# Begin Source File

SOURCE=..\SSHPUBK.C
# End Source File
# Begin Source File

SOURCE=..\SSHRAND.C
# End Source File
# Begin Source File

SOURCE=..\SSHRSA.C
# End Source File
# Begin Source File

SOURCE=..\SSHRSAG.C
# End Source File
# Begin Source File

SOURCE=..\SSHSH512.C
# End Source File
# Begin Source File

SOURCE=..\SSHSHA.C
# End Source File
# Begin Source File

SOURCE=..\SSHZLIB.C
# End Source File
# Begin Source File

SOURCE=..\TELNET.C
# End Source File
# Begin Source File

SOURCE=..\terminal.c
# End Source File
# Begin Source File

SOURCE=..\TREE234.C
# End Source File
# Begin Source File

SOURCE=..\UNICODE.C
# End Source File
# Begin Source File

SOURCE=..\WCWIDTH.C
# End Source File
# Begin Source File

SOURCE=..\WILDCARD.C
# End Source File
# Begin Source File

SOURCE=..\WINDEFS.C
# End Source File
# Begin Source File

SOURCE=..\WINDOW.C
# End Source File
# Begin Source File

SOURCE=..\winmisc.c
# End Source File
# Begin Source File

SOURCE=..\WINNET.C
# End Source File
# Begin Source File

SOURCE=..\WINSTORE.C
# End Source File
# Begin Source File

SOURCE=..\WINUTILS.C
# End Source File
# Begin Source File

SOURCE=..\X11FWD.C
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\INT64.H
# End Source File
# Begin Source File

SOURCE=..\LDISC.H
# End Source File
# Begin Source File

SOURCE=..\MISC.H
# End Source File
# Begin Source File

SOURCE=..\NETWORK.H
# End Source File
# Begin Source File

SOURCE=..\PROXY.H
# End Source File
# Begin Source File

SOURCE=..\PSFTP.H
# End Source File
# Begin Source File

SOURCE=..\PUTTY.H
# End Source File
# Begin Source File

SOURCE=..\PUTTYMEM.H
# End Source File
# Begin Source File

SOURCE=..\PUTTYPS.H
# End Source File
# Begin Source File

SOURCE=..\RESOURCE.H
# End Source File
# Begin Source File

SOURCE=..\SFTP.H
# End Source File
# Begin Source File

SOURCE=..\SSH.H
# End Source File
# Begin Source File

SOURCE=..\STORAGE.H
# End Source File
# Begin Source File

SOURCE=..\TERMINAL.H
# End Source File
# Begin Source File

SOURCE=..\TREE234.H
# End Source File
# Begin Source File

SOURCE=..\WIN_RES.H
# End Source File
# Begin Source File

SOURCE=..\WINHELP.H
# End Source File
# Begin Source File

SOURCE=..\WINSTUFF.H
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
