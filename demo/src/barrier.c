/* Demo 3 - three workers arrive at different times; nobody goes on
 * before everybody has arrived. The semaphore counts the missing workers:
 * each arrival subtracts 1, then waits until it is zero (sem_op = 0). */
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include "errExit.h"
#include "semaphore.h"

#define WORKERS 3

int main(int argc, char *argv[]) {
    int semid = semget(IPC_PRIVATE, 1, S_IRUSR | S_IWUSR);
    if (semid == -1)
        errExit("semget");
    union semun arg;
    arg.val = WORKERS;
    if (semctl(semid, 0, SETVAL, arg) == -1)
        errExit("semctl SETVAL");

    for (int w = 1; w <= WORKERS; w++) {
        pid_t pid = fork();
        if (pid == -1)
            errExit("fork");
        if (pid == 0) {
            sleep(w);                     // worker w arrives after w seconds
            printf("worker %d: arrived\n", w);
            fflush(stdout);
            semOp(semid, 0, -1);          // one worker less to wait for
            semOp(semid, 0, 0);           // wait until the count is zero
            printf("worker %d: go!\n", w);
            fflush(stdout);
            exit(0);
        }
    }

    // The parent watches the semaphore: its value and how many processes
    // are waiting for it to become zero
    for (int t = 0; t < WORKERS; t++) {
        sleep(1);
        int value = semctl(semid, 0, GETVAL, 0);
        int zwait = semctl(semid, 0, GETZCNT, 0);
        if (value == -1 || zwait == -1)
            errExit("semctl");
        printf("  [parent] missing %d, waiting for zero: %d\n", value, zwait);
        fflush(stdout);
    }
    while (wait(NULL) != -1)
        ;
    if (semctl(semid, 0, IPC_RMID, 0) == -1)
        errExit("semctl IPC_RMID");
    return 0;
}
