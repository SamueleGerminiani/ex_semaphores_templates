#ifndef _REPORT_HH
#define _REPORT_HH

/* Run report code: you do not need to read or modify this file :)
 *
 * report_licence is called by a job right after taking a licence
 * (acquired = 1) and right before releasing it (acquired = 0).
 * report_check reveals code #3 when the three licences were never
 * exceeded. The code is bound to the current user and machine.
 */
void report_licence(int job, int acquired);
void report_check(int semid);

#endif
