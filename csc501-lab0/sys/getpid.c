/* getpid.c - getpid */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include "lab0.h"

extern int syscall_start;
/*------------------------------------------------------------------------
 * getpid  --  get the process id of currently executing process
 *------------------------------------------------------------------------
 */
SYSCALL getpid()
{
	unsigned long starttime;
	starttime = ctr1000;
	int syscall_id = 2;
	if (syscall_start == 1) {
		syscall_table[currpid][syscall_id].call_name = "sys_getpid";
		syscall_table[currpid][syscall_id].call_freq += 1;
		syscall_table[currpid][syscall_id].call_time_taken += ctr1000 - starttime;
	}
	return(currpid);
}
