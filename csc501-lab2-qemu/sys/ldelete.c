/* ldelete.c - ldelete */

#include <stdio.h>
#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <lock.h>
#include <q.h>

/*------------------------------------------------------------------------
 * ldelete  --  delete a lock by removing the table entry
 *------------------------------------------------------------------------
 */
SYSCALL ldelete(int lock)
{
	STATWORD ps;    
	int	pid, i;
	struct	lentry	*lockptr;

	disable(ps);
	if(isbadlock(lock) || locks[lock].lstate==LFREE){
		restore(ps);
		return(SYSERR);
	}
	lockptr = &locks[lock];
	lockptr->lstate = LFREE;   // change state to FREE
	lockptr->lock_deleted = 1;	    // record the deleted info by storing 1 in the lock_deleted

	/* update the locks for process and mark them as DELETED in the proc-table */
	for(i=0; i< NPROC; i++){
		if(lockptr->lock_acquired[i]){
			lockptr->lock_acquired[i] = 0;
			proctab[i].plocksacquired[lock] = DELETED; 
		}
	}
	/* if there are processes waiting for the lock, wake them up */
	if(nonempty(lockptr->lqhead)){
        pid = getfirst(lockptr->lqhead);
        /* for every process update the pwaitret value, resched is called only when all the processes are unblocked*/
		while(pid != EMPTY){
		    proctab[pid].pwaitret = DELETED;
		    ready(pid,RESCHNO);
		}
		resched();
	}
	restore(ps);
	return(OK);
}
