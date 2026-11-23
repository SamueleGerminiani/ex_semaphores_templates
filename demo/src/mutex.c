/* Demo 1 - two workers write a three-line report on the same terminal.
 *   ./mutex_demo        no synchronisation: the lines of A and B interleave
 *   ./mutex_demo lock   a semaphore with value 1 protects each report */
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
  int lock = argc > 1;

  // A set with one semaphore, worth 1: one worker at a time
  int semid = semget(IPC_PRIVATE, 1, S_IRUSR | S_IWUSR);
  if (semid == -1) errExit("semget");
  union semun arg;
  arg.val = 1;
  if (semctl(semid, 0, SETVAL, arg) == -1) errExit("semctl SETVAL");

  printf("two workers, %s (semaphore set %d)\n",
         lock ? "with the lock" : "no lock", semid);

  fflush(stdout);
  for (int w = 0; w < 2; w++) {
    pid_t pid = fork();
    if (pid == -1) errExit("fork");
    if (pid == 0) {
      const char *name = (w == 0) ? "worker A" : "worker B";
      if (lock) semOp(semid, 0, -1);  // wait: enter the critical section
      for (int line = 1; line <= 3; line++) {
        printf("%s: line %d of 3\n", name, line);
        fflush(stdout);
        sleep(1);
      }
      if (lock) semOp(semid, 0, 1);  // signal: leave it
      exit(0);
    }
  }
  while (wait(NULL) != -1);
  if (semctl(semid, 0, IPC_RMID, 0) == -1) errExit("semctl IPC_RMID");
  return 0;
}
