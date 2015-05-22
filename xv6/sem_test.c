#include "types.h"
#include "stat.h"
#include "user.h"

#define SEM_NAME "Test"
#define FORK_NUM 20

int main(int argc, char *argv[])
{
	int sleep_counter;
	int i;
    int sd;
    int pid;

    /* initialize semaphores for shared processes */
    sd = sem_open(SEM_NAME, 1, 2, 2);

    if(sd < 0 ) {
        printf (2, "failed to create a semaphore!\n");
        exit();
    }

    printf (1, "created semaphore on index %d\n\n", sd);

    /* close prevents the semaphore existing forever */
    /* if a crash occurs during the execution         */
    printf (1, "semaphores initialized.\n\n");

    /* fork child processes */
    for (i = 0; i < FORK_NUM; i++){
        pid = fork ();
        if (pid < 0)              /* check for error      */
            printf (2, "Fork error.\n");
        else if (pid == 0)
            break;                  /* child processes */
    }

    // parent process
    if (pid != 0){
        /* wait for all children to exit */
        while (wait() > 0){
        }

        printf (1, "\n(%d)Parent: All children have exited.\n", getpid());

        /* cleanup semaphores */
        sem_close (sd);
        exit ();
    }

    // child process
    else{
        sem_wait (sd);           /* P operation */
        printf (1, "  (%d) Child(%d) is in critical section.\n", getpid(), i);
        for(sleep_counter = 0; sleep_counter < 10000000; sleep_counter++);

        printf (1, "  (%d) Child(%d) finished!\n", getpid(), i);
        sem_signal (sd);           /* V operation */
        exit ();
    }

	exit();
}
