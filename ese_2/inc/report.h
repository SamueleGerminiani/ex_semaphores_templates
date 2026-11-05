#ifndef _REPORT_HH
#define _REPORT_HH

/* Run report code: you do not need to read or modify this file :)
 *
 * report_check reveals code #2 when results.vcf holds one intact record
 * per worker. The code is bound to the current user and machine.
 */
void report_check(int semid, int workers);

#endif
