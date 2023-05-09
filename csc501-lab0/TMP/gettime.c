/* gettime.c - gettime */

#include <conf.h>
#include <kernel.h>
#include <date.h>
#include <proc.h>
#include <stdio.h>
#include "lab0.h"

extern int syscall_start;
extern int getutim(unsigned long *);

/*------------------------------------------------------------------------
 *  gettime  -  get local time in seconds past Jan 1, 1970
 *------------------------------------------------------------------------
 */
SYSCALL	gettime(long *timvar)
{
    /* long	now; */

	/* FIXME -- no getutim */
    unsigned long starttime;
	starttime = ctr1000;
	int syscall_id = 4;
	if (syscall_start == 1) {
		syscall_table[currpid][syscall_id].call_name = "sys_gettime";
		syscall_table[currpid][syscall_id].call_freq += 1;
    	syscall_table[currpid][syscall_id].call_time_taken += ctr1000 - starttime;
	}
    return OK;
}
