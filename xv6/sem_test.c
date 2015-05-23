#include "types.h"
#include "stat.h"
#include "user.h"

#define SEM_NAME "Test"
#define FORK_NUM 20

void test_create_semaphores();
void test_explicitly_close_semaphores();
void test_create_and_open_semaphores();
void test_close_via_exit();
void test_wait_and_signal();
void test_multiple_wait_and_signal();
void wastetime(int amount);

int main(int argc, char *argv[])
{
	printf(1, "\n****I am process %d, let the testing begin!****", getpid());

	printf(1, "\n\nTest1: create semaphores:\n");
	printf(1, "************************************\n");
	test_create_semaphores();
	wastetime(5000000);

	printf(1, "\n\nTest2: close semaphores explicitely (by calling sem_close):\n");
	printf(1, "************************************\n");
	test_explicitly_close_semaphores();
	wastetime(5000000);

	printf(1, "\n\nTest3: create and open semaphores:\n");
	printf(1, "************************************\n");
	test_create_and_open_semaphores();
	wastetime(5000000);

	printf(1, "\n\nTest4: close semaphores by exiting:\n");
	printf(1, "************************************\n");
	test_close_via_exit();
	wastetime(5000000);

	printf(1, "\n\nTest5: wait and signal:\n");
	printf(1, "************************************\n");
	test_wait_and_signal();
	wastetime(5000000);

	printf(1, "\n\nTest6: multiple signal and wait");
	test_multiple_wait_and_signal();
	wastetime(5000000);

	while(wait() > 0);
	printf(1, "I am the parent. My id is %d. I am exiting!\n", getpid());

	exit();
}

void test_create_semaphores()
{
	int semid1 = sem_open("aaa", 1, 4, 4);
	printf(1, "Semaphore aaa local id is %d.\n", semid1);
	int semid2 = sem_open("bbb", 1, 4, 4);
	printf(1, "Semaphore bbb local id is %d.\n", semid2);
	int semid3 = sem_open("ccc", 1, 2, 1);
	printf(1, "Semaphore ccc local id is %d (Should be -1).\n", semid3);

	wastetime(50000000);
}

void test_explicitly_close_semaphores()
{
	sem_close(0);
	sem_close(1);
	sem_close(2);

	int semid1 = sem_open("aaa", 0, 0, 0);
	printf(1, "Semaphore aaa local id is %d (Should be -1).\n", semid1);
	int semid2 = sem_open("bbb", 0, 0, 0);
	printf(1, "Semaphore bbb local id is %d (Should be -1).\n", semid2);
	int semid3 = sem_open("ccc", 0, 0, 0);
	printf(1, "Semaphore ccc local id is %d (Should be -1).\n", semid3);

	wastetime(50000000);
}

void test_create_and_open_semaphores()
{
	int k;
	int pid;

	int semid1 = sem_open("aaa", 1, 4, 3);
	printf(1, "Semaphore aaa local id is %d. (Should be -1)\n", semid1);
	int semid2 = sem_open("bbb", 1, 4, 4);
	printf(1, "Semaphore bbb local id is %d.\n", semid2);
	int semid3 = sem_open("ccc", 1, 2, 2);
	printf(1, "Semaphore ccc local id is %d.\n", semid3);

	for (k = 0; k < 2; k++)
	{
		pid = fork();

		if (pid == 0)
		{
			printf(1, "I am child with id: %d.\n", getpid());
			printf(1, "Child id: %d opening semaphore bbb.\n", getpid());
			int id_bbb = sem_open("bbb", 0, 0, 0);
			printf(1, "Opened id: %d.\n", id_bbb);

			printf(1, "Child id: %d opening semaphore aaa.\n", getpid());
			int id_ccc = sem_open("ccc", 0, 0, 0);
			printf(1, "Opened id: %d.\n", id_ccc);

			printf(1, "Child id: %d is exiting!.\n", getpid());
			exit();
		}
	}

	wastetime(50000000);
	sem_close(0);
	sem_close(1);
	sem_close(2);
}

void test_close_via_exit()
{
	int k;
	int pid;
	int semid1 = sem_open("aaa", 1, 4, 2);
	printf(1, "Semaphore aaa local id is %d.\n", semid1);
	int semid2 = sem_open("bbb", 1, 4, 5);
	printf(1, "Semaphore bbb local id is %d.\n", semid2);
	int semid3 = sem_open("ccc", 1, 2, 6);
	printf(1, "Semaphore ccc local id is %d.\n", semid3);

	for (k = 0; k < 3; k++)
	{
		pid = fork();

		if (pid == 0)
		{
			int s1 = sem_open("ddd", 1, 4, 4);
			printf(1, "Child created semaphore ddd. Local id is %d.\n", s1);
			int s2 = sem_open("eee", 1, 3, 4);
			printf(1, "Child created semaphore eee. Local id is %d.\n", s2);
			int s3 = sem_open("fff", 1, 2, 5);
			printf(1, "Child created semaphore fff. Local id is %d.\n", s3);

			printf(1, "Child id: %d is exiting!.\n", getpid());
			exit();
		}
	}

	wastetime(50000000);
	int id_ddd = sem_open("ddd", 0, 0, 0);
	printf(1, "Tried to open semaphore ddd, but got id: %d. Child closed it by exiting.\n", id_ddd);
	sem_close(0);
	sem_close(1);
	sem_close(2);
}

void test_wait_and_signal()
{
	int k;
	int pid;
	int sd = sem_open("ttt", 1, 4, 5);
	printf(1, "Semaphore ttt local id is %d.\n", sd);

	for (k = 0; k < 2; k++)
	{
		pid = fork();

		if (pid == 0)
		{
			int s1 = sem_open("ttt",0 ,0 ,0);
			printf(1, "Child opened semaphore ttt. Local id is %d.\n", s1);
			sem_wait(s1);

			printf(1, "Child id: %d is exiting!.\n", getpid());
			sem_signal(s1);
			wastetime(50000000);
			exit();
		}
	}

	wastetime(10000000);
	sem_wait(sd);
	printf(1, "Parent releasing ttt.\n");
	sem_signal(sd);
}

void test_multiple_wait_and_signal()
{
	int i;
	int sd;
	int pid;

	// initialize semaphores for shared processes
	sd = sem_open(SEM_NAME, 1, 2, 2);

	if(sd < 0 ) {
		printf (2, "failed to create a semaphore!\n");
		exit();
	}

	printf (1, "created semaphore on index %d\n\n", sd);
	printf (1, "semaphores initialized.\n\n");

	// fork child processes
	for (i = 0; i < FORK_NUM; i++){
		pid = fork ();

		// Check for error
		if (pid < 0)
			printf (2, "Fork error.\n");

		// parent process
		if (pid != 0){
			if(i % 3 == 0) {
				printf(1, "  (%d)Parent: I'm waiting!.\n", getpid());
				sem_wait(sd);
				printf(1, "  (%d)Parent: is in critical section (%d).\n", getpid(), i);
				wastetime(10000000);

				printf(1, "  (%d)Parent: finished (%d)!.\n", getpid(), i);
				sem_signal(sd);
			}
		}

		// child process
		else{
			sem_wait (sd);
			printf (1, "  (%d) Child(%d) is in critical section.\n", getpid(), i);
			wastetime(10000000);

			printf (1, "  (%d) Child(%d) finished!\n", getpid(), i);
			sem_signal (sd);
			exit ();
		}                 /* child processes */
	}

	// wait for all children to exit
	while (wait() > 0){
	}

	printf (1, "\n(%d)Parent: All children have exited.\n", getpid());

	// cleanup semaphores
	sem_close (sd);
}

// Simulate a sleeping process by running a loop for a given amount of iterations.
void wastetime(int amount)
{
	int i;
	for (i = 0; i < amount; i++);
}
