#ifndef _REPORT_HH
#define _REPORT_HH

/* Run report code: you do not need to read or modify this file :)
 *
 * report_stage is called by every stage after its work on a sample.
 * report_check reveals code #1 when every sample went through the four
 * stages in order. The code is bound to the current user and machine.
 */
void report_stage(int stage, int sample);
void report_check(int semid, int samples);

#endif
