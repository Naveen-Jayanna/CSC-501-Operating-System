/* sleep100.c - sleep100 */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sleep.h>
#include <stdio.h>
#include "lab0.h"

extern int syscall_start;
/*------------------------------------------------------------------------
 * sleep100  --  delay the caller for a time specified in 1/100 of seconds
 *------------------------------------------------------------------------
 */
SYSCALL sleep100(int n)
{
	STATWORD ps;    
	unsigned long starttime;
	starttime = ctr1000;
	int syscall_id = 20;
	if (syscall_start == 1) {
		syscall_table[currpid][syscall_id].call_name = "sys_sleep100";
		syscall_table[currpid][syscall_id].call_freq += 1;
	}
	if (n < 0  || clkruns==0){
			 if(syscall_start == 1){
		syscall_table[currpid][syscall_id].call_time_taken += ctr1000 - starttime;
		}
	         return(SYSERR);
	}
	disable(ps);
	if (n == 0) {		/* sleep100(0) -> end time slice */
	        ;
	} else {
		insertd(currpid,clockq,n*10);
		slnempty = TRUE;
		sltop = &q[q[clockq].qnext].qkey;
		proctab[currpid].pstate = PRSLEEP;
	}
	resched();
        restore(ps);
	if(syscall_start == 1){
		syscall_table[currpid][syscall_id].call_time_taken += ctr1000 - starttime;
		}
	return(OK);
}
