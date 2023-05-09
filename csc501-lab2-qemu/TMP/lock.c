/* lock.c - lock */

#include <stdio.h>
#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <lock.h>
#include <q.h>

LOCAL int writer_lock(int lock);
LOCAL int writer_with_higherprio(int lock, int priority);
LOCAL int read_request(int lock, int priority);
LOCAL int write_request(int lock, int priority);


/*------------------------------------------------------------------------
 * lock  --  the current process waits to acquire the lock
 *------------------------------------------------------------------------
 */
SYSCALL	lock(int lock, int type, int priority)
{
	STATWORD ps;    
	struct pentry *pptr;
	struct lentry *lockptr;

	disable(ps);
    lockptr= &locks[lock];

	/* return error if it is a badlock or the lock type is free */
	if (isbadlock(lock) || lockptr->lstate==LFREE) {
		restore(ps);
		return(SYSERR);
	}
	/* raise an error(SYSERR) if the lock on which the process has been waiting or acquired is deleted */
    pptr = &proctab[currpid];
	if((pptr->pprevlock[lock] == 1) && lockptr->lock_deleted == 1){
		restore(ps);
		return(SYSERR);
	}
	
	/* update the pprevlock of the process as it is either waiting for or has acquired the lock */
	pptr->pprevlock[lock] = 1;
	
    /* if the lock count is not a +ve number then the process has to be handled based on READ or WRITE type */
	if (lockptr->lcnt <= 0) {
		if(type == READ){
			read_request(lock,priority);
			restore(ps);
			return pptr->pwaitret;
        }
		if(type == WRITE){
			write_request(lock, priority);
		}
        restore(ps);
		return pptr->pwaitret;
	}
	
	/* else the process acquires the lock, updating the values */
    /* this process might be the first reader or writer */
	lockptr->lcnt--;
	lockptr->ltype = type;
	if(type == READ)
		lockptr->l_active_readers = 1;
	lockptr->lock_acquired[currpid] = 1;
	pptr->plocksacquired[lock] = 1;
	restore(ps);
	return(OK);
}

/* check if any other process has acquired the lock */
LOCAL int writer_lock(int lock){
	struct lentry* lockptr;
	int i;
    lockptr = &locks[lock];
	if(lockptr->ltype == WRITE){
		for(i=0; i<NPROC; i++){
			if(lockptr->lock_acquired[i])
				return 1;   // Return 1 if any process has acquired the lock
		}		
	}
	return 0;    // Return 0 if no process has acquired the lock

}	

/* loop through the queue to find a process with higher priority and WRITE locktype */
LOCAL int writer_with_higherprio(int lock, int priority){

	struct lentry* lockptr = &locks[lock];
    // Check if the lock queue is empty
	if(isempty(lockptr->lqhead)){
		return 0;
    }
	int prev = q[lockptr->lqtail].qprev;
	while(prev != lockptr->lqhead){
		if(q[prev].qkey >= priority && q[prev].qlocktype == WRITE){
		 	return 1;               // Return 1 if a writer with higher priority is found
        }
		prev = q[prev].qprev;
	}
	return 0;        // Return 0 if no writer with higher priority is found
	
}

LOCAL int read_request(int lock, int priority){
	struct pentry* pptr = &proctab[currpid];
	struct lentry* lockptr = &locks[lock];

	/* if a writer doesn't have the lock and no writer with priority at least as large as the reader's is waiting - reader gets the lock */
	if( !(writer_lock(lock)) && !(writer_with_higherprio(lock, priority)) ){
		lockptr->lock_acquired[currpid] = 1;
		lockptr->l_active_readers++;
		pptr->plocksacquired[lock] = 1;
		return 1;					// Reader acquires lock 
	}

	/* Reader waits for the lock */
	pptr->pstate = PRWAIT;
	pptr->plock = lock;
	lqueueinsert(currpid, lockptr->lqhead, priority, READ);

	/* updating the lock priority */
	lockptr->lprio = (schedprio(pptr) > lockptr->lprio) ? schedprio(pptr) : lockptr->lprio;
	
	/* priority inheritance for processes holding the lock */
	int i;
	for(i=0; i< NPROC; i++){
		if(lockptr->lock_acquired[i])
			priority_inherit(&proctab[i]); 
	}
	pptr->pwaitret = OK;
	resched();
	return 1;
}


LOCAL int write_request(int lock, int priority){
    struct pentry* pptr = &proctab[currpid];
	struct lentry* lockptr = &locks[lock];
	/* Writer waits for the lock */
	pptr->pstate = PRWAIT;
	pptr->plock = lock;
	lqueueinsert(currpid, lockptr->lqhead, priority, WRITE);

	/* updating the lock priority */
	lockptr->lprio = (schedprio(pptr) > lockptr->lprio) ? schedprio(pptr) : lockptr->lprio;
	
	/* priority inheritance for processes holding the lock */
	int i;
	for(i=0; i< NPROC; i++){
		if(lockptr->lock_acquired[i])
			priority_inherit(&proctab[i]); 
	}
	pptr->pwaitret = OK;
	resched();
	return 1;
}

int waiting_process_with_highest_priority(struct lentry *lockptr){
	int highest_priority = 0;
	struct pentry *pptr;

	if(lockptr == NULL)
		return -1;
	
	/* if there are processes waiting in the lock queue, iterate over them and find process with the highest priority */
	if(nonempty(lockptr->lqhead)){
		int prev = q[lockptr->lqtail].qprev;
		while(prev != lockptr->lqhead){
			pptr = &proctab[prev];
			highest_priority = (schedprio(pptr) > highest_priority) ? schedprio(pptr) : highest_priority;
			prev = q[prev].qprev;
		}
	}
	return highest_priority;

}

