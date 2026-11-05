#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "errExit.h"
#include "report.h"
#include "semaphore.h"

// Prints the value of every semaphore of the set (given)
void printState(int semid, int nsems) {
    unsigned short values[64];
    union semun arg;
    arg.array = values;
    // Read all the values at once using 'semctl' and GETALL
    if (semctl(semid, 0, GETALL, arg) == -1)
        errExit("semctl GETALL");
    printf("semaphore set %d:", semid);
    for (int i = 0; i < nsems; i++)
        printf(" [%d] = %d", i, values[i]);
    printf("\n");
    fflush(stdout);
}

int main(int argc, char *argv[]) {
    // Check command line input arguments using 'argc'
    if (argc != 2) {
        printf("Usage: %s <chromosomes>\n", argv[0]);
        return 1;
    }

    // Convert argv[1] to an integer using 'atoi': the chromosomes, one
    // worker each
    int chromosomes = 0;
    if (chromosomes <= 0 || chromosomes > 24) {
        printf("The number of chromosomes must be between 1 and 24!\n");
        return 1;
    }

    // Create a semaphore set with one semaphore using 'semget', the key
    // IPC_PRIVATE and the permissions S_IRUSR | S_IWUSR
    // (use 'errExit' if it fails)
    int semid = -1;


    // Initialize the semaphore to the number of chromosomes using 'semctl'
    // and SETVAL: the workers still calling (use 'errExit' if it fails)
    union semun arg;
    arg.val = 0 /*change this*/;


    printState(semid, 1);

    // Create one worker per chromosome using 'fork'
    for (int chr = 1; chr <= chromosomes; chr++) {
        pid_t pid = -1;

        if (1 /*change this*/) {
            // fork failed: print a message using 'errExit'

        } else if (1 /*change this*/) {
            // Phase 1, calling: it takes a few seconds (given)
            sleep(1 + chr % 3);
            printf("chr%d: calls complete, waiting for the others\n", chr);
            fflush(stdout);
            report_phase(chr, 1);

            // Barrier: one worker less is calling (semOp with -1), then wait
            // until nobody is calling any more (semOp with 0)



            // Phase 2, merging
            printf("chr%d: merging\n", chr);
            fflush(stdout);
            report_phase(chr, 2);

        }
        // Parent: keep creating workers
    }

    // Parent: wait for the termination of every worker using 'wait'


    printState(semid, 1);

    // Run report (given): reveals code #4 when nobody merged too early
    report_check(semid, chromosomes);

    // Remove the semaphore set using 'semctl' and IPC_RMID
    // (use 'errExit' if it fails)


    return 0;
}
