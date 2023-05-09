/* chprio.c - chprio */

#include <lock.h>
#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <stdio.h>


/*------------------------------------------------------------------------
 * chprio  --  change the scheduling priority of a process
 *------------------------------------------------------------------------
 */
SYSCALL chprio(int pid, int newprio)
{
	STATWORD ps;    
	struct	pentry	*pptr;
	struct 	lentry	*lptr;
	int	i;

	pptr = &proctab[pid];
	disable(ps);
	if (isbadpid(pid) || newprio<=0 || pptr->pstate == PRFREE) {
		restore(ps);
		return(SYSERR);
	}

	pptr->pinh = pptr->pinh != 0 ? newprio : pptr->pinh; 
	pptr->pprio = newprio;	

	/* if the process is waiting, inherit the highest priority of the waiting processes */
	if(pptr->pstate == PRWAIT){
		lptr = &locks[pptr->plock];
        lptr->lprio = waiting_process_with_highest_priority(lptr);
		for(i=0; i<NPROC; i++){
			if(lptr->lock_acquired[i])
				priority_inherit(&proctab[i]);
		}
		
	}
	restore(ps);
	return(newprio);
}
