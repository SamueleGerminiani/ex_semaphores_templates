#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <unistd.h>

#include "errExit.h"
#include "semaphore.h"

int main(int argc, char *argv[]) {
    // Make the key of the queue using 'ftok', exactly as the pipeline does:
    // the current directory "." and the project id 'q'
    // (use 'errExit' if it fails)
    key_t key = -1;


    // Open the existing set using 'semget' with the key, 2 semaphores and
    // the flags 0 (no IPC_CREAT): it fails with "No such file or directory"
    // if the pipeline is not running (use 'errExit' if it fails)
    int semid = -1;


    printf("submit: PID %d submitting a sample to the queue (semid %d)\n", getpid(), semid);

    // Sign the submission on semaphore 1 (semOp with +1), then announce the
    // sample on semaphore 0 (semOp with +1): the pipeline wakes up



    return 0;
}
