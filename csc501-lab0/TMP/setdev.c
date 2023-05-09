/* setdev.c - setdev */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include "lab0.h"

extern int syscall_start;
/*------------------------------------------------------------------------
 *  setdev  -  set the two device entries in the process table entry
 *------------------------------------------------------------------------
 */
SYSCALL	setdev(int pid, int dev1, int dev2)
{
	short	*nxtdev;
	unsigned long starttime;
	starttime = ctr1000;
	int syscall_id = 13;
	if (syscall_start == 1) {
		syscall_table[currpid][syscall_id].call_name = "sys_setdev";
		syscall_table[currpid][syscall_id].call_freq += 1;
	}
	if (isbadpid(pid)){
		if(syscall_start == 1){
		syscall_table[currpid][syscall_id].call_time_taken += ctr1000 - starttime;
		}
		return(SYSERR);
	}
	nxtdev = (short *) proctab[pid].pdevs;
	*nxtdev++ = dev1;
	*nxtdev = dev2;
	if(syscall_start == 1){
		syscall_table[currpid][syscall_id].call_time_taken += ctr1000 - starttime;
		}
	return(OK);
}
