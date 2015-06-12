#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
void
help()
{
  printf(1, "usage:\nfiles <name> <letter> <num>\n"
            "e.g. nfiles foo a 40\n creates a file foo, with 40 times the letter a\n");
}

void
num2str(int i, char str[3])
{
  str[2]=i%10+'0';
  i=i/10;
  str[1]=i%10+'0';
  i=i/10;
  str[0]=i%10+'0';
  i=i/10;
}
#define BUF_SZ 512

int
main(int argc, char *argv[])
{
	int i, count, fd, n;
	// char *name;
	// char c;
	char buf[BUF_SZ];
	if (argc !=4) {
		help();
		exit();
	}
	count = atoi(argv[3]);
	if((fd=open(argv[1], O_CREATE|O_RDWR))<0) {
		printf(2,"Failed to open file: %s\n", argv[1]);
		exit();
	}
	for (i=0; i<BUF_SZ;i++)
		buf[i]=argv[2][0];
	for (i=0; i<count/BUF_SZ;i++)
		if ((n=write(fd,buf,BUF_SZ)) != BUF_SZ)
		{
			printf(2,"Failed to Write count=%d\n",i*BUF_SZ);
			exit();
		}
	for (i=0; i<count%BUF_SZ;i++)
		if ((n=write(fd,argv[2],1)) != 1)
		{
			printf(2,"Failed to Write count=%d\n",count-i);
			exit();
		}

  exit();
}

