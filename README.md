# xv6
OS Course assignments

#Ex 1
implemntation of the syscall get_proc_data :

      int get_proc_data(int pid, struct proc_data*);

where proc_data is defined as:

      struct proc_data {

        uint ppid;

        char name[16];

        uint size;

      };

Add user program called ps_prog:
ps_prog takes one argument integer argument X,
then it will create X son processes recursively,
after X processes have been created, the program will print the processes tree
using the get_proc_data syscall.

#Ex2

Added abillity to use named semaphores, using the following syscall API:

        int sem_open(char *name, int create, int init, int maxVal);
 
        int sem_close(int sd);
 
        int sem_wait(int sd);
 
        int sem_trywait(int sd);
 
        int sem_signal(int sd);
