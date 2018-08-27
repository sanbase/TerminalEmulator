// EnBitmap.cpp: implementation of the CImage class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CImage.h"
#include <Afxsock.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

const int HIMETRIC_INCH	= 2540;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CImage::CImage()
{
}

CImage::~CImage()
{ 

}
/*
BOOL CImage::LoadImage(UINT uIDRes, LPCTSTR szResourceType, CDC *dc, HMODULE hInst, COLORREF crBack)
{
	ASSERT(m_hObject == NULL);      // only attach once, detach on destroy

	if (m_hObject != NULL)
		return FALSE;

	BOOL bResult = FALSE;

	// some problems with bitmaps and icons
	// OleLoadPicture returns unknown error code
	if (szResourceType == RT_BITMAP)
	{
		if (!hInst)
			hInst = AfxGetInstanceHandle();

		bResult = CBitmap::Attach((HBITMAP)::LoadImage(hInst, MAKEINTRESOURCE(uIDRes), IMAGE_BITMAP, 0, 0, 0));
	}
	else if (szResourceType == RT_ICON)
	{
		if (!hInst)
			hInst = AfxGetInstanceHandle();

		HICON hIcon = (HICON)::LoadImage(hInst, MAKEINTRESOURCE(uIDRes), IMAGE_ICON, 0, 0, 0);

		if (hIcon)
		{
			bResult = Attach(hIcon);
			::DestroyIcon(hIcon);
		}
	}
	else
	{
		BYTE*	pBuff = NULL;
		FileSize = 0;

		// first call is to get buffer size
		if (GetResource(MAKEINTRESOURCE(uIDRes), szResourceType, hInst, 0, FileSize))
		{
			if (FileSize > 0)
			{
				pBuff = new BYTE[FileSize];
				
				// this loads it
				if (GetResource(MAKEINTRESOURCE(uIDRes), szResourceType, hInst, pBuff, FileSize))
				{
					bResult = LoadFromBuffer(pBuff, FileSize, dc);
				}				
				delete [] pBuff;
			}
		}
	}

	return bResult;
}
*/
BOOL CImage::LoadImage(LPCTSTR szImagePath, CDC *dc, COLORREF crBack)
{
	ASSERT(m_hObject == NULL);      // only attach once, detach on destroy
	BOOL bResult = FALSE;

	if (m_hObject != NULL)
		return FALSE;

	CFile			cFile;
	CFileException	e;
	
	if (cFile.Open(szImagePath, CFile::modeRead | CFile::typeBinary, &e))
	{
		FileSize = (int)cFile.GetLength();

		BYTE* pBuff = new BYTE[FileSize];
		if (cFile.Read(pBuff, FileSize) > 0)
		{
			bResult=LoadFromBuffer(pBuff, FileSize, dc);			
		}
		delete [] pBuff;
	}
	return bResult;
}

BOOL CImage::LoadFromBuffer(BYTE* pBuff, int nSize, CDC *dc)
{
	bool bResult = false;

	HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, nSize);
	void* pData = GlobalLock(hGlobal);
	memcpy(pData, pBuff, nSize);
	GlobalUnlock(hGlobal);

	IStream* pStream = NULL;
	IPicture* pPicture = NULL;

	if (CreateStreamOnHGlobal(hGlobal, TRUE, &pStream) == S_OK)
	{
		HRESULT hr = OleLoadPicture(pStream, nSize, FALSE, IID_IPicture, (LPVOID *)&pPicture);
		pStream->Release();
		bResult = hr==S_OK ;
		if(hr==S_OK)
		{
			HBITMAP hbm=0;
			if(pPicture!=NULL)
			{
				pPicture->get_Handle((OLE_HANDLE*)&hbm);			
				Attach(hbm);
			}
		}
	}
	return bResult; // caller releases
}
/*
BOOL CImage::GetResource(LPCTSTR lpName, LPCTSTR lpType, HMODULE hInst, void* pResource, int& nBufSize)
{ 
	HRSRC		hResInfo;
	HANDLE		hRes;
	LPSTR		lpRes	= NULL; 
	int			nLen	= 0;
	bool		bResult	= FALSE;

	hResInfo = FindResource(hInst, lpName, lpType);

	if (hResInfo == NULL) 
		return false;
	hRes = LoadResource(hInst, hResInfo);
	if (hRes == NULL) 
		return false;
	lpRes = (char*)LockResource(hRes);
	if (lpRes != NULL)
	{ 
		if (pResource == NULL)
		{
			nBufSize = SizeofResource(hInst, hResInfo);
			bResult = true;
		}
		else
		{
			if (nBufSize >= (int)SizeofResource(hInst, hResInfo))
			{
				memcpy(pResource, lpRes, nBufSize);
				bResult = true;
			}
		} 

		UnlockResource(hRes);  
	}
	FreeResource(hRes);

	return bResult;
}
*/
BOOL CImage::LoadFromURL(LPCSTR host,CString name,CDC *dc)
{
    int size;
    char Buf[4096];
	char *tmp=NULL;

	CSocket m_hSocket;
	m_hSocket.Create();
	FileSize=0;		
	if(m_hSocket.Connect(host,80)==false)
    {
		return(false);
    }
    CString str = "GET "+name+"\n\r\n\r";
	if(m_hSocket.Send(str,str.GetLength())==SOCKET_ERROR)
	{
		m_hSocket.Close();
		return false;
	}
    while((size=m_hSocket.Receive(Buf,sizeof Buf))>0)
    {
		tmp=(char *)realloc(tmp,FileSize+size);
		memcpy(tmp+FileSize,Buf,size);
		FileSize+=size;
    }
    m_hSocket.Close();
	BOOL res=LoadFromBuffer((unsigned char *)tmp,FileSize,dc);
	free(tmp);
	return(res);
}
