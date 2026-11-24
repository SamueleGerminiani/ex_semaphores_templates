/* Demo 2 - two processes take turns: one semaphore each.
 * The parent may print only when semaphore 0 is 1, the child when
 * semaphore 1 is 1; each one passes the turn to the other. */
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include "errExit.h"
#include "semaphore.h"

int main(int argc, char *argv[]) {
    int semid = semget(IPC_PRIVATE, 2, S_IRUSR | S_IWUSR);
    if (semid == -1)
        errExit("semget");

    // Semaphore 0 (the parent's turn) starts at 1, semaphore 1 at 0
    unsigned short initial[2] = {1, 0};
    union semun arg;
    arg.array = initial;
    if (semctl(semid, 0, SETALL, arg) == -1)
        errExit("semctl SETALL");

    pid_t pid = fork();
    if (pid == -1)
        errExit("fork");

    for (int round = 1; round <= 3; round++) {
        if (pid == 0) {
            semOp(semid, 1, -1);          // wait for my turn
            printf("      pong %d (child %d)\n", round, getpid());
            fflush(stdout);
            semOp(semid, 0, 1);           // your turn
        } else {
            semOp(semid, 0, -1);
            printf("ping %d (parent %d)\n", round, getpid());
            fflush(stdout);
            semOp(semid, 1, 1);
        }
    }
    if (pid == 0)
        exit(0);
    wait(NULL);
    if (semctl(semid, 0, IPC_RMID, 0) == -1)
        errExit("semctl IPC_RMID");
    return 0;
}
