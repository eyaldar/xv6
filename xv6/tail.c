#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

char buf[512];

void
tail(int fd, int n)
{
  while((n = read(fd, buf, sizeof(buf))) > 0)
    write(1, buf, n);
  if(n < 0){
    printf(1, "tail: read error\n");
    exit();
  }
}

int
main(int argc, char *argv[])
{
	int fd, n;
	char filename[10];
	struct stat stat;

	if(argc != 3){
		printf(1, "tail: invalid number of arguments\n");
		exit();
	}

	strcpy(filename, argv[1]);
	n=atoi(argv[2]);

	if((fd = open(filename, O_RDONLY | O_RDWR)) < 0){
		printf(1, "tail: cannot open %s\n", filename);
		exit();
	}

	if(n > 0) {
		fstat(fd, &stat);
		lseek(fd, stat.size - n, '\0');
		tail(fd, n);
	}

    close(fd);

	exit();
}
