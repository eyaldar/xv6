#include "types.h"
#include "x86.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

// Fetch the nth word-sized system call argument as a semaphore descriptor
// and return both the descriptor and the corresponding struct semaphore.
static int
argsd(int n, int *psd, struct sem **ps)
{
  int sd;
  struct sem *s;

  if(argint(n, &sd) < 0)
    return -1;
  if(sd < 0 || sd >= NOSEM || (s=proc->osem[sd]) == 0)
    return -1;
  if(psd)
    *psd = sd;
  if(ps)
    *ps = s;
  return 0;
}

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return proc->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = proc->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;
  
  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(proc->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;
  
  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}


int
sys_sem_open(void)
{
	char* name;
	int create;
	int init;
	int maxVal;

	if(argstr(0, &name) < 0)
		return -1;

	if(argint(1, &create) < 0)
		return -1;

	if(argint(2, &init) < 0)
		return -1;

	if(argint(3, &maxVal) < 0)
		return -1;

	return sem_open(name, create, init, maxVal);
}

int
sys_sem_close(void)
{
	int sd;
	struct sem *s;

	if(argsd(0, &sd, &s) < 0)
		return -1;
	proc->osem[sd] = 0;

	return sem_close(s);
}

int
sys_sem_wait(void)
{
	struct sem *s;

	if(argsd(0, 0, &s) < 0)
		return -1;

	return sem_wait(s);
}

int
sys_sem_trywait(void)
{
	struct sem *s;

	if(argsd(0, 0, &s) < 0)
		return -1;

	return sem_trywait(s);
}

int
sys_sem_signal(void)
{
	struct sem *s;

	if(argsd(0, 0, &s) < 0)
		return -1;

	return sem_signal(s);
}
