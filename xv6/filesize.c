#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int
main(int argc, char *argv[])
{
	int fd;
	char filename[10];
	struct stat stat;

	if(argc != 2){
		printf(1, "filesize: invalid number of arguments\n");
		exit();
	}

	strcpy(filename, argv[1]);

	if((fd = open(filename, O_RDONLY | O_RDWR)) < 0){
		printf(1, "filesize: cannot open %s\n", filename);
		exit();
	}

	fstat(fd, &stat);
	printf(1, "size of file %s is: %d\n", filename, stat.size);

    close(fd);

	exit();
}
