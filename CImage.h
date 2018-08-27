// EnBitmap.h: interface for the CImage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ENBITMAP_H__1FDE0A4E_8AB4_11D6_95AD_EFA89432A428__INCLUDED_)
#define AFX_ENBITMAP_H__1FDE0A4E_8AB4_11D6_95AD_EFA89432A428__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CImage : public CBitmap  
{
public:
	CImage();
	virtual ~CImage();
	int FileSize;

	CBitmap pBmpMask;

	BOOL LoadImage(LPCTSTR szImagePath, CDC *dc, COLORREF crBack = 0);
//	BOOL LoadImage(UINT uIDRes, LPCTSTR szResourceType, CDC *dc, HMODULE hInst = NULL, COLORREF crBack = 0); 
	BOOL LoadFromBuffer(BYTE* pBuff,int nSize, CDC *dc);
	BOOL LoadFromURL(LPCTSTR host, CString name, CDC *dc);
	// helpers
//	static BOOL GetResource(LPCTSTR lpName, LPCTSTR lpType, HMODULE hInst, void* pResource, int& nBufSize);
protected:
//	BOOL Attach(HICON hIcon, COLORREF crBack);

};

#endif // !defined(AFX_ENBITMAP_H__1FDE0A4E_8AB4_11D6_95AD_EFA89432A428__INCLUDED_)
