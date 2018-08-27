// Termcap.h: interface for the Termcap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TERMCAP_H__A252DD35_3844_49CC_9EB5_A04709A45842__INCLUDED_)
#define AFX_TERMCAP_H__A252DD35_3844_49CC_9EB5_A04709A45842__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Termcap
{
private:
	char *tbuf;
	char *bufc;
	int hopcount;   /* detect infinite loops in termcap, init 0 */
	char *cbuf;

	char *tdecode(char *str,char **area);
	char *tskip(char *bp);
	int tgetent(char *bufc, char *name);
	int tnchktc();
	int tnamatch(char *np);
public:
	Termcap(char *,char *);
	virtual ~Termcap();
	int tgetnum(char *id);
	int tgetflag(char *id);
	char *tgetstr(char *id);

};


#endif // !defined(AFX_TERMCAP_H__A252DD35_3844_49CC_9EB5_A04709A45842__INCLUDED_)
