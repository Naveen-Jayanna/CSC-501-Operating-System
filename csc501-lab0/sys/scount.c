/* scount.c - scount */

#include <conf.h>
#include <kernel.h>
#include <sem.h>
#include <proc.h>
#include "lab0.h"

extern int syscall_start;
/*------------------------------------------------------------------------
 *  scount  --  return a semaphore count
 *------------------------------------------------------------------------
 */
SYSCALL scount(int sem)
{
extern	struct	sentry	semaph[];
	unsigned long starttime;
	starttime = ctr1000;
	int syscall_id = 10;
	if (syscall_start == 1) {
		syscall_table[currpid][syscall_id].call_name = "sys_scount";
		syscall_table[currpid][syscall_id].call_freq += 1;
	}

	if (isbadsem(sem) || semaph[sem].sstate==SFREE){
		if(syscall_start == 1){
		syscall_table[currpid][syscall_id].call_time_taken += ctr1000 - starttime;
		}
		return(SYSERR);
	}
	if(syscall_start == 1){
		syscall_table[currpid][syscall_id].call_time_taken += ctr1000 - starttime;
		}
	return(semaph[sem].semcnt);
}
