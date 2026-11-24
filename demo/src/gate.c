/* Demo 4 - two unrelated processes meet on a semaphore through a key.
 *   terminal 1:  ./gate_demo        creates the set (key from ftok), closes
 *                                   the gate and waits for three cars
 *   terminal 2:  ./gate_demo open   opens the same set by key and lets one
 *                                   car through: run it three times
 * No fork: the only thing the two processes have in common is the key. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <unistd.h>

#include "errExit.h"
#include "semaphore.h"

#define CARS 3

int main(int argc, char *argv[]) {
    // The same existing file and the same project id give the same key in
    // every program: the Makefile of this directory and the number 1.
    // Start both roles from this directory, or ftok does not find the file
    key_t key = ftok("Makefile", 1);
    if (key == -1)
        errExit("ftok");

    if (argc > 1 && strcmp(argv[1], "open") == 0) {
        // The opener: open the existing set, never create it (flags 0).
        // If the gate is not running the set does not exist: ENOENT
        int semid = semget(key, 1, 0);
        if (semid == -1)
            errExit("semget");
        semOp(semid, 0, 1);               // let one car through
        printf("PID %d: opened the gate, set %d (key 0x%x)\n", getpid(), semid, key);
        return 0;
    }

    // The gate: create the set; IPC_EXCL fails if a previous run left it behind
    int semid = semget(key, 1, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    if (semid == -1)
        errExit("semget");
    union semun arg;
    arg.val = 0;                          // 0: the gate is closed
    if (semctl(semid, 0, SETVAL, arg) == -1)
        errExit("semctl SETVAL");
    printf("PID %d: gate closed, set %d (key 0x%x)\n", getpid(), semid, key);
    printf("  from another terminal: ./gate_demo open\n");
    fflush(stdout);

    for (int car = 1; car <= CARS; car++) {
        semOp(semid, 0, -1);              // wait until somebody opens
        printf("car %d passed\n", car);
        fflush(stdout);
    }

    if (semctl(semid, 0, IPC_RMID, 0) == -1)
        errExit("semctl IPC_RMID");
    printf("removed the set %d\n", semid);
    return 0;
}
