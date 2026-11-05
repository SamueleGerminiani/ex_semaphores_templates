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

#define STAGES 4
const char *stageName[STAGES] = {"align", "sort", "call", "annotate"};

int main(int argc, char *argv[]) {
    // Check command line input arguments using 'argc'
    if (argc != 2) {
        printf("Usage: %s <samples>\n", argv[0]);
        return 1;
    }

    // Convert argv[1] to an integer using 'atoi': the samples of the run
    int samples = 0;
    if (samples <= 0) {
        printf("The number of samples must be greater than 0!\n");
        return 1;
    }

    // Create a semaphore set with STAGES semaphores using 'semget', the key
    // IPC_PRIVATE and the permissions S_IRUSR | S_IWUSR
    // (use 'errExit' if it fails)
    int semid = -1;


    // Initialize the set using 'semctl' and SETALL: the semaphore of the
    // first stage starts at 1, the others at 0 (use 'errExit' if it fails)
    unsigned short initial[STAGES] = {0 /*change this*/};
    union semun arg;
    arg.array = initial;


    printf("pipeline: %d sample(s) through %d stages\n", samples, STAGES);
    fflush(stdout);   // the buffer would be duplicated by fork

    // Create one child process per stage using 'fork'
    for (int stage = 0; stage < STAGES; stage++) {
        pid_t pid = -1;

        if (1 /*change this*/) {
            // fork failed: print a message using 'errExit'

        } else if (1 /*change this*/) {
            // Stage: for every sample, wait for its turn on the semaphore
            // 'stage' (semOp with -1), print the line, then pass the sample
            // on by signalling the semaphore of the next stage,
            // (stage + 1) % STAGES (semOp with +1)
            for (int k = 1; k <= samples; k++) {


                printf("sample %d: %s\n", k, stageName[stage]);
                fflush(stdout);
                report_stage(stage, k);


            }
            exit(0);
        }
        // Parent: keep creating stages
    }

    // Parent: wait for the termination of every stage using 'wait'


    // Run report (given): reveals code #1 when the stages ran in order
    report_check(semid, samples);

    // Remove the semaphore set using 'semctl' and IPC_RMID
    // (use 'errExit' if it fails)


    return 0;
}
