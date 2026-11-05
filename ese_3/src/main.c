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

#define JOBS 8
#define LICENCES 3

int main(int argc, char *argv[]) {
    // Create a semaphore set with one semaphore using 'semget', the key
    // IPC_PRIVATE and the permissions S_IRUSR | S_IWUSR
    // (use 'errExit' if it fails)
    int semid = -1;


    // Initialize the semaphore to LICENCES using 'semctl' and SETVAL
    // (use 'errExit' if it fails)
    union semun arg;
    arg.val = 0 /*change this*/;


    printf("licence server: %d licences for %d alignment jobs\n", LICENCES, JOBS);
    fflush(stdout);   // the buffer would be duplicated by fork

    // Create one process per job using 'fork'
    for (int job = 1; job <= JOBS; job++) {
        pid_t pid = -1;

        if (1 /*change this*/) {
            // fork failed: print a message using 'errExit'

        } else if (1 /*change this*/) {
            // Job: take a licence (semOp with -1: it blocks while the three
            // licences are in use)


            printf("job %d: licence acquired, aligning\n", job);
            fflush(stdout);
            report_licence(job, 1);

            // Align: it takes a few seconds (given)
            sleep(1 + job % 3);

            printf("job %d: done, releasing the licence\n", job);
            fflush(stdout);
            report_licence(job, 0);

            // Release the licence (semOp with +1) and terminate with 'exit'


        }
        // Parent: keep creating jobs
    }

    // Parent (licence server): until every job has terminated, collect the
    // terminated jobs with 'waitpid' (-1 and WNOHANG: it returns 0 while
    // every job is still running) and, once per second, print the state of
    // the licences: "licence server: free <GETVAL>, waiting <GETNCNT>" using
    // 'semctl' (use 'errExit' if a call fails)
    int done = 0;
    while (done < JOBS) {
        pid_t pid = -1;







        int free = -1;
        int waiting = -1;

        printf("licence server: free %d, waiting %d\n", free, waiting);
        fflush(stdout);

        sleep(1);
    }

    // Run report (given): reveals code #3 when the licences were respected
    report_check(semid);

    // Remove the semaphore set using 'semctl' and IPC_RMID
    // (use 'errExit' if it fails)


    return 0;
}
