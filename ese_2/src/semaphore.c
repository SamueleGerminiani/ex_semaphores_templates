#include <sys/sem.h>

#include "semaphore.h"
#include "errExit.h"

void semOp (int semid, unsigned short sem_num, short sem_op) {
    struct sembuf sop;
    //Populate the sembuf structure
    //for example: sop.sem_num = sem_num;

    //run the semop system call
    if (semop(/*...*/) == /*...*/)
        errExit("semop failed");
}
