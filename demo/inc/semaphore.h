#ifndef _SEMAPHORE_HH
#define _SEMAPHORE_HH

#include <sys/sem.h>

// The union semun must be defined by the programmer (see semctl)
// only if not on macos
#if !defined(__APPLE__)
union semun {
  int val;
  struct semid_ds *buf;
  unsigned short *array;
};
#endif

/* semOp performs the operation sem_op on the semaphore sem_num of the set
 * semid: sem_op < 0 waits (P), sem_op > 0 signals (V), sem_op == 0 waits
 * until the semaphore is zero. It terminates the process with errExit if
 * semop fails. */
void semOp(int semid, unsigned short sem_num, short sem_op);

#endif
