#include "semaphore.h"

#include <sys/sem.h>

#include "errExit.h"

void semOp(int semid, unsigned short sem_num, short sem_op) {
    struct sembuf sop;

    // Fill the three fields of sop: sem_num, sem_op and sem_flg (0)
    sop.sem_num = sem_num;
    sop.sem_op = sem_op;
    sop.sem_flg = 0;

    // Perform the single operation with 'semop' (use 'errExit' if it fails)
    if (semop(semid, &sop, 1) == -1)
        errExit("semop");
}
