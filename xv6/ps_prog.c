#include "types.h"
#include "stat.h"
#include "user.h"

int main(int argc, char *argv[])
{
	int my_pid = getpid();
	int parent_pid = getppid();

    printf(1, "My Pid is: %d\n",my_pid);
    printf(1, "My Parent Pid is %d\n", parent_pid);
    exit();
}
