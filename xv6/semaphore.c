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
	int available_locks;	// Number of available users.
	int max_locks;			// Number of maximum users.

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

// Get or create a named semaphore with the given name
// inside the system semaphore table.
struct sem*
get_sem(char* name, int create, int init, int maxVal) {
	struct sem *s;
	struct sem *empty_s;

	acquire(&stable.gslock);
	for (s = stable.sem; s < stable.sem + NSEM; s++) {
		// if the semaphore is not in use is empty
		if (s->ref == 0) {
			if (empty_s != 0) {
				empty_s = s;
			}
		} else {
			// if name was found
			if (memcmp(s->name, name, NSEM_NAME)) {
				s->ref++;
				release(&stable.gslock);
				return s;
			}
		}
	}

	// if create is allowed and found inactive entry
	if (create && empty_s != 0) {
		s = empty_s;

		safestrcpy(s->name, name, sizeof(name));
		s->ref = 1;
		s->available_locks = init;
		s->max_locks = maxVal;
		initlock(&s->sslock, s->name);

		release(&stable.gslock);
		return s;
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
	struct sem *s;

	// name should not exceed the name limit.
	if (strlen(name) > NSEM_NAME)
		return -1;

	// get semaphore by name
	if ((s = get_sem(name, create, init, maxVal)) == 0
			|| (sd = sdalloc(s)) < 0) {
		if (s) {
			sem_close(s);
		}
		return -1;
	}

	return sd;
}

// Close semaphore s.  (Decrement ref count, close when reaches 0.)
int sem_close(struct sem* s) {
	acquire(&stable.gslock);

	if (s->ref < 0)
		panic("sem_close");
	if (--s->ref > 0) {
		release(&stable.gslock);
		return 0;
	}

	s->ref = 0;
	s->max_locks = 0;
	s->available_locks = 0;

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

		sleep(&s->available_locks, &s->sslock);  //DOC: pipewrite-sleep
	}

	release(&s->sslock);
	return 0;
}

// Decrement the semaphore available locks,
// in case there are no available locks, will return -1.
// this method doesn't lock the semaphore
int sem_trywait_nolocks(struct sem* s) {
	if (s->available_locks > 0) {
		s->available_locks--;
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

	if (s->available_locks == s->max_locks) {
		release(&s->sslock);
		return -1;
	}

	s->available_locks++;
	wakeup(&s->available_locks);
	release(&s->sslock);

	return 0;
}
