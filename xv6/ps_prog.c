#include "types.h"
#include "stat.h"
#include "user.h"

void create_process_tree(int depth);
void print_process_tree(int pid);

int main(int argc, char *argv[])
{
	int depth;

	if(argc != 2){
		printf(2, "ps_prog: expecting a single int argument\n");
		exit();
	}

	depth = atoi(argv[1]);

	printf(1, "depth=%d\n", depth);
	create_process_tree(depth);

	exit();
}

void create_process_tree(int depth)
{
	if(depth == 0)
	{
		printf(1, "PID NAME SIZE\n");
		print_process_tree(getpid());

		return;
	}

	int pid;

	pid = fork();

	if(pid == 0)
	{
		create_process_tree(depth - 1);
	}
	else
	{
		wait();
		exit();
	}
}

void print_process_tree(int pid)
{
	struct proc_data proc_data;

	if(pid == 0)
		return;

	get_proc_data(pid, &proc_data);
	printf(1, "%d %s %d\n", pid, proc_data.name, proc_data.size);

	print_process_tree(proc_data.ppid);
}


