/* Demo 4 - an IPC object lives in the kernel, not in the process.
 *   ./leftover_demo         creates a set with a key made by ftok, waits 30 s
 *   Ctrl+C                  the process dies, the set stays: ipcs -s
 *   ./leftover_demo         again: IPC_EXCL makes semget fail, "File exists"
 *   ./leftover_demo clean   removes it (ipcrm -s <id> does the same) */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <unistd.h>

#include "errExit.h"
#include "semaphore.h"

int main(int argc, char *argv[]) {
    // The same pathname and project id always give the same key
    key_t key = ftok(".", 'x');
    if (key == -1)
        errExit("ftok");

    if (argc > 1 && strcmp(argv[1], "clean") == 0) {
        // Open the existing set (no IPC_CREAT) and remove it
        int semid = semget(key, 1, 0);
        if (semid == -1)
            errExit("semget (nothing to clean?)");
        if (semctl(semid, 0, IPC_RMID, 0) == -1)
            errExit("semctl IPC_RMID");
        printf("removed the set %d (key 0x%x)\n", semid, key);
        fflush(stdout);
        return 0;
    }

    // IPC_EXCL: fail if a set with this key already exists
    int semid = semget(key, 1, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    if (semid == -1)
        errExit("semget");

    printf("PID %d: created the set %d with key 0x%x\n", getpid(), semid, key);

    fflush(stdout);
    printf("  ipcs -s shows it. Ctrl+C now, then run me again...\n");
    fflush(stdout);
    sleep(30);

    if (semctl(semid, 0, IPC_RMID, 0) == -1)
        errExit("semctl IPC_RMID");
    printf("removed the set %d\n", semid);
    fflush(stdout);
    return 0;
}
