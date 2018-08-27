// Termcap.cpp: implementation of the Termcap class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Termcap.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#include <io.h>
#include <fcntl.h>

#define TBUFSIZE  4096
#define MAXHOP  32      /* max number of tc= indirections */
static char *err_mes="The termcap is too long!\n";
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

extern CString path;
Termcap::Termcap(char *term, char *stat_buf)
{
	cbuf=stat_buf;
	bufc=(char *)calloc(TBUFSIZE,1);
	hopcount=0;

	if(!tgetent(bufc, term))
	{
		free(bufc);
		exit(0);
	}
}

Termcap::~Termcap()
{
	if(bufc!=NULL)
		free(bufc);
}

int Termcap::tgetent(char *bp,char *name)
{
	register char *cp;
	register int c;
	register int i = 0, cnt = 0;
	int skip = 0;
	char *ibuf;
//	char *cp2;
	int tf;
	char *termc="/termcap";

	ibuf=(char *)malloc(TBUFSIZE);
	tbuf = bp;
	tf = 0;
	strcpy(ibuf,path+"etc/termcap");
	tf = open(ibuf,O_RDONLY|O_TEXT);
	if (tf < 0)
	{
		AfxMessageBox("Can't open "+path+"etc/termcap");
		free(ibuf);
		return (-1);
	}
	for (;;)
	{
		cp = bp;
		for (;;)
		{
			if (i == cnt)
			{
				cnt = read(tf, ibuf, TBUFSIZE);
				if (cnt <= 0)
				{
					close(tf);
					free(ibuf);
					return (0);
				}
				i = 0;
			}
			c = ibuf[i++];
			if (c == '\n')
			{
				if (cp > bp && cp[-1] == '\\')
				{
					cp--;
					skip++;
					continue;
				}
				break;
			}
			if (skip && (c == ' ' || c == '\t'))
				continue;
			skip = 0;
			if (cp >= bp+TBUFSIZE)
			{
				write(2,err_mes, 23);
				break;
			}
			else
				*cp++ = c;
		}
		*cp = 0;

		/**
		* The real work for the match.
		*/
		if (tnamatch(name))
		{
			close(tf);
			free(ibuf);
			return(tnchktc());
		}
	}
}

/**
 * tnchktc: check the last entry, see if it's tc=xxx. If so,
 * recursively find xxx and append that entry (minus the names)
 * to take the place of the tc=xxx entry. This allows termcap
 * entries to say "like an HP2621 but doesn't turn on the labels".
 * Note that this works because of the left to right scan.
 */
int Termcap::tnchktc()
{
	register char *p, *q;
	char tcname[64];        /* name of similar terminal */

	char *tcbuf;
	char *holdtbuf=tbuf;
	int l;

	tcbuf=(char *)malloc(TBUFSIZE);

	p = tbuf + strlen(tbuf) - 2;    /* before the last colon */

	while (*--p != ':')
		if (p<tbuf)
		{
			write(2, "Bad termcap\n", 12);
			free(tcbuf);
			return (0);
		}
	p++;
	/** p now points to beginning of last field */
	if (p[0] != 't' || p[1] != 'c')
	{
		free(tcbuf);
		return(1);
	}
	strcpy(tcname,p+3);
	q = tcname;
	while (*q && *q != ':')
		q++;
	*q = 0;
	if (++hopcount > MAXHOP)
	{
		write(2, "tc=error!\n", 10);
		free(tcbuf);
		return (0);
	}
	if (tgetent(tcbuf, tcname) != 1)
	{
		free(tcbuf);
		return(0);
	}
	for (q=tcbuf; *q != ':'; q++)
		;
	l = p - holdtbuf + strlen(q);
	if (l > TBUFSIZE)
	{
		write(2, err_mes, 23);
		q[TBUFSIZE - (p-tbuf)] = 0;
	}
	strcpy(p, q+1);
	tbuf = holdtbuf;
	free(tcbuf);
	return(1);
}

/**
 * Tnamatch deals with name matching.  The first field of the termcap
 * entry is a sequence of names separated by |'s, so we compare
 * against each such name.  The normal : terminator after the last
 * name (before the first field) stops us.
 */
int Termcap::tnamatch(char *np)
{
	register char *Np, *Bp;

	Bp = tbuf;
	if (*Bp == '#')
		return(0);
	for (;;)
	{
		for (Np = np; *Np && *Bp == *Np; Bp++, Np++)
			continue;
		if (*Np == 0 && (*Bp == '|' || *Bp == ':' || *Bp == 0))
			return (1);
		while (*Bp && *Bp != ':' && *Bp != '|')
			Bp++;
		if (*Bp == 0 || *Bp == ':')
			return (0);
		Bp++;
	}
}

/**
 * Skip to the next field.  Notice that this is very dumb, not
 * knowing about \: escapes or any such.  If necessary, :'s can be put
 * into the termcap file in octal.
 */
char *Termcap::tskip(char *bp)
{

	while (*bp && *bp != ':')
		bp++;
	while (*bp == ':')
		bp++;
	return (bp);
}

/**
 * Return the (numeric) option id.
 * Numeric options look like
 *      li#80
 * i.e. the option string is separated from the numeric value by
 * a # character.  If the option is not found we return -1.
 * Note that we handle octal numbers beginning with 0.
 */
int Termcap::tgetnum(char *id)
{
	register int i, base;
	register char *bp = tbuf;

	for (;;)
	{
		bp = tskip(bp);
		if (*bp == 0)
			return (-1);
		if (*bp++ != id[0] || *bp == 0 || *bp++ != id[1])
			continue;
		if (*bp == '@')
			return(-1);
		if (*bp != '#')
			continue;
		bp++;
		base = 10;
		if (*bp == '0')
			base = 8;
		i = 0;
		while (isdigit(*bp))
			i *= base, i += *bp++ - '0';
		return (i);
	}
}

/**
 * Handle a flag option.
 * Flag options are given "naked", i.e. followed by a : or the end
 * of the buffer.  Return 1 if we find the option, or 0 if it is
 * not given.
 */
int Termcap::tgetflag(char *id)
{
	register char *bp = tbuf;

	for (;;)
	{
		bp = tskip(bp);
		if (!*bp)
			return (0);
		if (*bp++ == id[0] && *bp != 0 && *bp++ == id[1])
		{
			if (!*bp || *bp == ':')
				return (1);
			else if (*bp == '@')
				return(0);
		}
	}
}

/**
 * Get a string valued option.
 * These are given as
 *      cl=^Z
 * Much decoding is done on the strings, and the strings are
 * placed in area, which is a ref parameter which is updated.
 * No checking on area overflow.
 */
char *Termcap::tgetstr(char *id)
{
	char **area=&cbuf;
	register char *bp = tbuf;
	for (;;)
	{
		bp = tskip(bp);
		if (!*bp)
			return (NULL);
		if (*bp++ != id[0] || *bp == 0 || *bp == ':' || *bp++ != id[1])
			continue;
		if (*bp == '@')
			return(NULL);
		if (*bp != '=')
			continue;
		bp++;
		return (tdecode(bp, area));
	}
}

/**
 * Tdecode does the grung work to decode the
 * string capability escapes.
 */
char *Termcap::tdecode(char *str,char **area)
{
	register char *cp;
	register int c;
	register char *dp;
	int i;
	/**
	int jdelay=0;
		while(*str>='0' && *str<='9')
		{
			jdelay=jdelay*10+(*str++ - '0');
		}
	*/
	cp = *area;
	while ((c = *str++) && c != ':')
	{
		switch (c)
		{

		case '^':
			c = *str++ & 037;
			break;

		case '\\':
			dp = "E\033^^\\\\::n\nr\rt\tb\bf\f";
			c = *str++;
nextc:
			if (*dp++ == c)
			{
				c = *dp++;
				break;
			}
			dp++;
			if (*dp)
				goto nextc;
			if (isdigit(c))
			{
				c -= '0', i = 2;
				do
				    c <<= 3, c |= *str++ - '0';
				while (--i && isdigit(*str));
			}
			break;
		}
		*cp++ = c;
	}
	*cp++ = 0;
	str = *area;
	*area = cp;
	return (str);
}
