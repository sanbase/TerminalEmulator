#include <stdio.h>
#include <fcntl.h>
#include <sys/param.h>
#define BSIZE 1024
main(argc,argv)
int argc;
char **argv;
{
	int i,j,size;
	unsigned char ch[BSIZE];
	char name[256];
	if(argc<2)
		printf("Вызов: dos_conv список файлов\n");
	while((--argc)>0)
	{
		if((i=open(argv[argc],O_RDWR))<0) continue;
		j=creat("....",0600);
		fprintf(stderr,"\r%s                \r",argv[argc]);
		while((size=read(i,ch,BSIZE))>0)
		{
			register int i;
			for(i=0;i<size;i++)
				ch[i]=dos_xenix(ch[i]);
			write(j,ch,size);
		}
		close(j);
		close(i);
		unlink(argv[argc]);
		bzero(name,sizeof name);
		for(i=0;argv[argc][i];i++)
		{
			if(argv[argc][i]>='A' && argv[argc][i]<='Z')
				name[i]=argv[argc][i]+'a'-'A';
			else    name[i]=argv[argc][i];
		}
		link("....",name);
		unlink("....");
	}
	exit(0);
}
dos_xenix(i)
unsigned char i;
{
	register int s;
	if(i!='\r')
		return(i);
	return(' ');
}
