#include "types.h"
#include "stat.h"
#include "user.h"

int main(int argc, char *argv[])
{
	sem_open("hi", 1, 2, 3);

	exit();
}
