// String.h: interface for the String class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STRING_H__85362F34_4B92_4867_B970_9BA4DE7A1EE6__INCLUDED_)
#define AFX_STRING_H__85362F34_4B92_4867_B970_9BA4DE7A1EE6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class String: public CString
{
public:
	String();
	String(char *str);
	int Length();
	void SetLength(int a);
	char *c_str();
};


#endif // !defined(AFX_STRING_H__85362F34_4B92_4867_B970_9BA4DE7A1EE6__INCLUDED_)
