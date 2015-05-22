#include "types.h"
#include "stat.h"
#include "user.h"

int main(int argc, char *argv[])
{
	printf(1, "(hi, 1, 2, 3):\n");
	int sd = sem_open("hi", 1, 2, 3);
	printf(1, "sd = %d\n", sd);

	printf(1, "(hi, 0, 2, 3):\n");
	sd = sem_open("hi", 0, 2, 3);
	printf(1, "sd = %d\n", sd);

	printf(1, "(bye, 0, 2, 3):\n");
	sd = sem_open("bye", 0, 2, 3);
	printf(1, "sd = %d\n", sd);

	printf(1, "(bye, 1, 1, 0):\n");
	sd = sem_open("bye", 1, 1, 0);
	printf(1, "sd = %d\n", sd);

	printf(1, "(bye, 1, 1, 2):\n");
	sd = sem_open("bye", 1, 1, 2);
	printf(1, "sd = %d\n", sd);

	exit();
}
