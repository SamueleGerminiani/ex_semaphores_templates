#ifndef _REPORT_HH
#define _REPORT_HH

#include <sys/types.h>

/* Run report code: you do not need to read or modify this file :)
 *
 * report_check reveals code #5 when the queue was created with the key of
 * this directory and every sample was submitted by another process.
 * The code is bound to the current user and machine.
 */
void report_check(int semid, key_t key, int samples);

#endif
