#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "errExit.h"
#include "report.h"
#include "semaphore.h"

// The queue is a set of two semaphores:
//   [0] samples submitted and not yet processed (the pipeline waits on it)
//   [1] the signature: only submitters operate on it, so GETPID tells who
//       submitted last

int main(int argc, char *argv[]) {
    // Check command line input arguments using 'argc'
    if (argc != 2) {
        printf("Usage: %s <samples>\n", argv[0]);
        return 1;
    }

    // Convert argv[1] to an integer using 'atoi': the samples to process
    int samples = 0;
    if (samples <= 0) {
        printf("The number of samples must be greater than 0!\n");
        return 1;
    }

    // Make the key of the queue using 'ftok' with the current directory "."
    // and the project id 'q' (use 'errExit' if it fails)
    key_t key = -1;


    // Create the set with two semaphores using 'semget', the key and the
    // flags IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR. If a previous run left
    // the set behind, semget fails with "File exists": remove it with ipcrm
    // (use 'errExit' if it fails)
    int semid = -1;


    // Initialize both semaphores to 0 using 'semctl' and SETALL: no sample
    // submitted yet (use 'errExit' if it fails)
    unsigned short initial[2] = {0, 0};
    union semun arg;
    arg.array = initial;


    printf("pipeline: queue ready (key 0x%x, semid %d), waiting for %d sample(s)\n", key, semid, samples);
    printf("pipeline: submit a sample from another terminal with ./submit\n");
    fflush(stdout);

    for (int s = 1; s <= samples; s++) {
        // Wait for a submission on semaphore 0 (semOp with -1)


        // Who submitted it? The PID of the last process that operated on
        // semaphore 1: 'semctl' with GETPID (use 'errExit' if it fails)
        pid_t from = -1;


        printf("pipeline: sample %d received from PID %d, processing\n", s, from);
        fflush(stdout);
        sleep(2);
        printf("pipeline: sample %d done\n", s);
        fflush(stdout);
    }

    // Run report (given): reveals code #5 when every sample came through the queue
    report_check(semid, key, samples);

    // Remove the semaphore set using 'semctl' and IPC_RMID
    // (use 'errExit' if it fails)

    printf("pipeline: queue removed\n");

    return 0;
}
