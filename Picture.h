// Picture.h: interface for the CPicture class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PICTURE_H__C9846786_DFE7_4D0D_BC40_F00BD60131A6__INCLUDED_)
#define AFX_PICTURE_H__C9846786_DFE7_4D0D_BC40_F00BD60131A6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <atlbase.h>

//////////////////
// Picture object--encapsulates IPicture
//
class CPicture 
{
public:
	CPicture();
	~CPicture();

	// Load frm various sosurces
	BOOL Load(UINT nIDRes);
	BOOL Load(LPCTSTR pszPathName);
	BOOL Load(CFile& file);
	BOOL Load(CArchive& ar);
	BOOL Load(IStream* pstm);

	// render to device context
	BOOL Render(CDC* pDC, CRect rc=CRect(0,0,0,0),
		LPCRECT prcMFBounds=NULL) const;

	CSize GetImageSize(CDC* pDC=NULL) const;

	operator IPicture*() 
	{
		return m_spIPicture;
	}

	void GetHIMETRICSize(OLE_XSIZE_HIMETRIC& cx, OLE_YSIZE_HIMETRIC& cy) const 
	{
		cx = cy = 0;
		const_cast<CPicture*>(this)->m_hr = m_spIPicture->get_Width(&cx);
		ASSERT(SUCCEEDED(m_hr));
		const_cast<CPicture*>(this)->m_hr = m_spIPicture->get_Height(&cy);
		ASSERT(SUCCEEDED(m_hr));
	}

	void Free() 
	{
		if (m_spIPicture) 
		{
			m_spIPicture.Release();
		}
	}

protected:
	CComQIPtr<IPicture>m_spIPicture; // ATL smart pointer to IPicture
	HRESULT m_hr;  // last error code
};



#endif // !defined(AFX_PICTURE_H__C9846786_DFE7_4D0D_BC40_F00BD60131A6__INCLUDED_)
