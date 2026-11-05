#include <fcntl.h>
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

// The record of one chromosome: four lines, one second apart (given).
// The standard output is a file: without fflush, printf would keep the
// lines in its buffer and write them all at once when the process exits
void writeRecord(int chr) {
    printf("# chr%d: calling started by PID %d\n", chr, getpid());
    fflush(stdout);
    sleep(1);
    printf("chr%d %d A>G\n", chr, chr * 1000 + 17);
    fflush(stdout);
    sleep(1);
    printf("chr%d %d C>T\n", chr, chr * 1000 + 42);
    fflush(stdout);
    sleep(1);
    printf("# chr%d: calling complete\n", chr);
    fflush(stdout);
}

int main(int argc, char *argv[]) {
    // Check command line input arguments using 'argc'
    if (argc != 2) {
        printf("Usage: %s <workers>\n", argv[0]);
        return 1;
    }

    // Convert argv[1] to an integer using 'atoi': the number of workers,
    // one per chromosome
    int workers = 0;
    if (workers <= 0 || workers > 8) {
        printf("The number of workers must be between 1 and 8!\n");
        return 1;
    }

    // Create a semaphore set with one semaphore using 'semget', the key
    // IPC_PRIVATE and the permissions S_IRUSR | S_IWUSR
    // (use 'errExit' if it fails)
    int semid = -1;


    // Initialize the semaphore to 1 using 'semctl' and SETVAL: one worker
    // at a time in the file (use 'errExit' if it fails)
    union semun arg;
    arg.val = 0 /*change this*/;


    // Redirect the standard output to the file results.vcf: close
    // STDOUT_FILENO, then open results.vcf with the flags
    // O_WRONLY | O_CREAT | O_TRUNC and the mode S_IRUSR | S_IWUSR. The lowest
    // free descriptor is 1: from now on printf writes to the file
    // (use 'errExit' if a call fails)



    // Messages for the operator go to the standard error, which is still
    // the terminal
    fprintf(stderr, "variant calling: %d worker(s) writing results.vcf\n", workers);

    // Create one worker per chromosome using 'fork'
    for (int chr = 1; chr <= workers; chr++) {
        pid_t pid = -1;

        if (1 /*change this*/) {
            // fork failed: print a message using 'errExit'

        } else if (1 /*change this*/) {
            // Worker: take the semaphore (semOp with -1), write the record
            // of the chromosome with writeRecord, release the semaphore
            // (semOp with +1), terminate with 'exit'

            writeRecord(chr);


        }
        // Parent: keep creating workers
    }

    // Parent: wait for the termination of every worker using 'wait'


    fprintf(stderr, "run complete: results.vcf written\n");

    // Run report (given): reveals code #2 when every record is intact
    report_check(semid, workers);

    // Remove the semaphore set using 'semctl' and IPC_RMID
    // (use 'errExit' if it fails)


    return 0;
}
