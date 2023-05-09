/* lcreate.c - lcreate */

#include <stdio.h>
#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <lock.h>
#include <q.h>

 /*------------------------------------------------------------------------
  * createlock  --  create a new unused lock and return the index
  *------------------------------------------------------------------------
  */
LOCAL int createlock()
{
	int	i, lock;

	for(i=0; i<NLOCKS; i++){
		lock=nextlock;
		nextlock-=1;
        /* wrap around for nextlock value */
		if(nextlock < 0)
			nextlock = NLOCKS-1;

		// kprintf("\n nextlock = %d", NLOCKS);

        /* if the lock is free, lock_deleted != 1, lock created for the first time, else lock was deleted and remains the same till the end of execution */
		if(locks[lock].lstate==LFREE){
			if(locks[lock].lock_deleted != 1)
				locks[lock].lock_deleted = 0;
			locks[lock].lstate = LUSED;
			// kprintf("\n lock = %d", lock);
			// kprintf("\n syserr = %d", SYSERR);
			return(lock);
		}
	}
	// kprintf("end statement");
	return(SYSERR);
}

 /*------------------------------------------------------------------------
  * lcreate  --  create and initialize a lock, return lock descriptor
  *------------------------------------------------------------------------
  */
SYSCALL lcreate(void)
{
	STATWORD ps;    
	int i, lock;

	disable(ps);
    lock=createlock();         // create a new lock
	// kprintf("\n lcreate lock = %d", lock);
	if(lock==SYSERR){        // restore ps and return SYSERR on error
		restore(ps);
		return(SYSERR);
	}
    /* initialize total readers, priority to 0 and count to 1   */
	locks[lock].l_active_readers = 0;
	locks[lock].lprio = 0;
	locks[lock].lcnt = 1;
	
    /* initialize lock acquired for all the processes to 0 */
	for(i=0; i<NPROC; i++){
		locks[lock].lock_acquired[i] = 0;
    }
	restore(ps);
	return(lock);
}
