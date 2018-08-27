// String.cpp: implementation of the String class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "String.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

typedef CString String
#define c_str() GetBuffer(0)
/*
int String::Length()
{
	return(GetLength());
}

void String::SetLength(int a)
{
	if(a==0)
		Empty();
}

char *String::c_str()
{
	return(GetBuffer(0));
}
*/