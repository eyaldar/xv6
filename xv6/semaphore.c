//
// Semaphore descriptors
//
#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "defs.h"
#include "spinlock.h"

// Named semaphore
struct sem {
	struct spinlock sslock;	// single semaphore lock
	int ref;		 		// Reference counter
	int value;				// Number of available locks.
	int max;				// Number of maximum locks.

	char name[NSEM_NAME];	// Name of semaphore.
};

// Semaphore table
struct {
	struct spinlock gslock; // global semaphore lock
	struct sem sem[NSEM];	// semaphores array
} stable;

// Allocate a semaphore descriptor for the given semaphore.
static int sdalloc(struct sem *s) {
	int sd;

	for (sd = 0; sd < NOSEM; sd++) {
		if (proc->osem[sd] == 0) {
			proc->osem[sd] = s;
			return sd;
		}
	}
	return -1;
}

// initialize the semaphore table.
void sem_init(void) {
	initlock(&stable.gslock, "stable");
}

// Searches for a named semaphore by a given name
// inside the system semaphore table.
struct sem*
sem_find(char* name)
{
	struct sem *s;

	int name_len = strlen(name);

	acquire(&stable.gslock);
	for (s = stable.sem; s < stable.sem + NSEM; s++) {
		// if the semaphore is not in use is empty
		if (s->ref > 0 && (memcmp(s->name, name, name_len) == 0)) {
			cprintf("Found %s!",s->name);
			s->ref++;
			release(&stable.gslock);
			return s;
		}
	}

	release(&stable.gslock);
	return 0;
}

// Allocates a semaphore inside the semaphore table,
// in case there is no empty slots, will return -1.
struct sem*
sem_allocate(char* name, int init, int maxVal)
{
	struct sem *s;

	acquire(&stable.gslock);

	for (s = stable.sem; s < stable.sem + NSEM; s++) {
		// if the semaphore is not in use is empty
		if (s->ref == 0) {

			s->ref = 1;
			safestrcpy(s->name, name, strlen(name) + 1);
			s->value = init;
			s->max = maxVal;
			initlock(&s->sslock, "semaphore");

			release(&stable.gslock);
			return s;
		}
	}

	release(&stable.gslock);
	return 0;
}

// Increment ref count for semaphore s.
struct sem*
sem_dup(struct sem *s) {
	acquire(&stable.gslock);
	if (s->ref < 1)
		panic("sem_dup");
	s->ref++;
	release(&stable.gslock);
	return s;
}

// opens a semaphore by name, in case the semaphore doesn't
// exist and create is set to 1, will create a new semaphore
// with the values init and maxValue.
// in case fails will return 0, otherwise will return
// the semaphore descriptor index.
int sem_open(char* name, int create, int init, int maxVal) {
	int sd;
	int name_len;
	struct sem *s;

	name_len = strlen(name);

	// name should not exceed the name size limit.
	if (name_len > NSEM_NAME || name_len < 1) {
		cprintf("Invalid name %s\n", name);
		return -1;
	}

	// maxVal should be lower than init
	if (init > maxVal) {
		cprintf("Invalid init(%d) and maxVal(%d)\n", init, maxVal);
		return -1;
	}

	// If both find and create(when enabled) failed to return a semaphore.
	if ((s = sem_find(name)) == 0 && (!create || (s = sem_allocate(name, init, maxVal)) == 0)) {
		cprintf("Failed to find or allocate a semaphore\n");
		return -1;
	}

	// Trying to allocate semaphore descriptor for the process.
	if((sd = sdalloc(s)) < 0) {
		sem_close(s);
		cprintf("Failed to allocate a descriptor\n");
		return -1;
	}

	return sd;
}

// Close semaphore s.  (Decrement ref count, close when reaches 0.)
int sem_close(struct sem* s) {
	acquire(&stable.gslock);

	if (s->ref < 1)
		panic("sem_close");

	s->ref--;

	release(&stable.gslock);

	return 0;
}

// Decrement the semaphore available locks,
// in case there are no available locks, will go to sleep
// until any locks will become available.
int sem_wait(struct sem* s) {
	acquire(&s->sslock);

	while (sem_trywait_nolocks(s) < 0) {
		if (proc->killed) {
			release(&s->sslock);
			return -1;
		}

		sleep(&s->value, &s->sslock);  //DOC: pipewrite-sleep
	}

	release(&s->sslock);
	return 0;
}

// Decrement the semaphore available locks,
// in case there are no available locks, will return -1.
// this method doesn't lock the semaphore
int sem_trywait_nolocks(struct sem* s) {
	if (s->value > 0) {
		s->value--;
		return 0;
	}

	return -1;
}

// Decrement the semaphore available locks,
// in case there are no available locks, will return -1.
// this method lock the semaphore
int sem_trywait(struct sem* s) {
	int result;

	acquire(&s->sslock);
	result = sem_trywait_nolocks(s);
	release(&s->sslock);

	return result;
}

// Increment the semaphore available locks,
// in case there are maximum number of locks, will return -1.
int sem_signal(struct sem* s) {
	acquire(&s->sslock);

	if (s->value == s->max) {
		release(&s->sslock);
		return -1;
	}

	s->value++;
	wakeup(&s->value);
	release(&s->sslock);

	return 0;
}
