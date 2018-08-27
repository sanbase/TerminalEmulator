// This is a Winsock server that is listening on a port.

// When a client connects, the server spawns a child process and
// passes the socket handle to the child.
// The child can use this socket handle to interact with the
// client and the parent is free to go back to waiting for
// other clients to connect.

OrigSock=accept(listen_socket,(struct sockaddr *)&from,&fromlen);

if (OrigSock == INVALID_SOCKET)  
{
	fprintf(stderr,"accept failed %d\n",GetLastError());
	return -1;
}
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	char argbuf[256];

	memset(&si,0,sizeof(si));

//
// Duplicate the socket OrigSock to create an inheritable copy.
//
	if (!DuplicateHandle(GetCurrentProcess(),
	(HANDLE)OrigSock,
	GetCurrentProcess(),
	(HANDLE*)&DuplicateSock,
	0,
TRUE, // Inheritable
	DUPLICATE_SAME_ACCESS)) 
	{

		fprintf(stderr,"dup error %d\n",GetLastError());
		return -1;
	}
//
// Spawn the child process.
// The first command line argument (argv[1]) is the socket handle.
//

	wsprintf(argbuf,"child.exe %d",DuplicateSock);
	if (!CreateProcess(NULL,argbuf,NULL,NULL,
TRUE, // inherit handles
	0,NULL,NULL,&si,&pi) )
	{
		fprintf(stderr,"createprocess failed %d\n",GetLastError());
		return -1;
	}

//
// On Windows 95, the parent needs to wait until the child
// is done with the duplicated handle before closing it.
//
	WaitForSingleObject(pi.hProcess, INFINITE);
}
//

// The duplicated socket handle must be closed by the owner
// process--the parent. Otherwise, socket handle leakage
// occurs. On the other hand, closing the handle prematurely
// would make the duplicated handle invalid in the child. In this
// sample, we use WaitForSingleObject(pi.hProcess, INFINITE) to
// wait for the child.
//
closesocket(OrigSock);
closesocket(DuplicateSock);


The following sample code illustrates how the newly created process extracts the socket handle from its command line:

main(int argc, char *argv[])
{
	SOCKET Sock;

	/** WSAStartup etc. */
	if (2 == argc)
	{
		Sock = atoi(argv[1]);   // use Sock
	}
}
