#ifndef _REPORT_HH
#define _REPORT_HH

/* Run report code: you do not need to read or modify this file :)
 *
 * report_phase is called by a worker when its calls are complete
 * (phase 1) and when it starts merging (phase 2).
 * report_check reveals code #4 when no merge started before every
 * chromosome was called. The code is bound to the current user and machine.
 */
void report_phase(int chr, int phase);
void report_check(int semid, int chromosomes);

#endif
