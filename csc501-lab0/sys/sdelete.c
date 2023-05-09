/* sdelete.c - sdelete */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>
#include <stdio.h>
#include "lab0.h"

extern int syscall_start;
/*------------------------------------------------------------------------
 * sdelete  --  delete a semaphore by releasing its table entry
 *------------------------------------------------------------------------
 */
SYSCALL sdelete(int sem)
{
	STATWORD ps;    
	int	pid;
	struct	sentry	*sptr;

	unsigned long starttime;
	starttime = ctr1000;
	int syscall_id = 11;
	if (syscall_start == 1) {
		syscall_table[currpid][syscall_id].call_name = "sys_sdelete";
		syscall_table[currpid][syscall_id].call_freq += 1;
	}

	disable(ps);
	if (isbadsem(sem) || semaph[sem].sstate==SFREE) {
		restore(ps);
		if(syscall_start == 1){
		syscall_table[currpid][syscall_id].call_time_taken += ctr1000 - starttime;
		}
		return(SYSERR);
	}
	sptr = &semaph[sem];
	sptr->sstate = SFREE;
	if (nonempty(sptr->sqhead)) {
		while( (pid=getfirst(sptr->sqhead)) != EMPTY)
		  {
		    proctab[pid].pwaitret = DELETED;
		    ready(pid,RESCHNO);
		  }
		resched();
	}
	restore(ps);
	if(syscall_start == 1){
		syscall_table[currpid][syscall_id].call_time_taken += ctr1000 - starttime;
		}
	return(OK);
}
