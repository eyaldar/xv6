#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

char buf[512]={0};

int atoi_with_neg(char *s) {
	if (*s=='-')
		return (-1 * atoi(++s));
	else
		return atoi(s);
}
		
void fill_buf(char c, int count){
	int i;
	
	for (i=0; i<count; i++)
		buf[i]=c;
	buf[i]=0;	
}

int
main(int argc, char *argv[])
{
  int fd, pos, read_count, write_count, seek, ret;
  char c;
  char filename[10];

  strcpy(filename, argv[1]);
  read_count=atoi(argv[2]);
  seek=atoi_with_neg(argv[3]);
  c = *argv[4];
  write_count=atoi(argv[5]);
  
  if((fd = open(filename, O_CREATE | O_RDWR)) < 0){
      printf(1, "cat: cannot open %s\n", filename);
      exit();
  }
  if (read_count > 0)
	  ret=read(fd, buf, read_count);
  printf(1, "read %d bytes, str is: %s\n", ret, buf);
  printf(1, "running lseek(%d)\n", seek);
  pos=lseek(fd, seek, '\0');
  printf(1, "return value/position = %d\n", pos);
  if (write_count > 0) {
	  fill_buf(c, write_count);

	  ret=write(fd, buf, write_count);
	  printf(1, "buf = %s, just wrote %d bytes\n", buf, ret);
  }
  close(fd);
  exit();
}
