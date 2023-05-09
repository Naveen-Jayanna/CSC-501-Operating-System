/* releaseall.c - releaseall */

#include <stdio.h>
#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <lock.h>
#include <q.h>

/*--------------------------------------------------------------------------
 * releaseall  --  release the acquired locks by the current process
 *--------------------------------------------------------------------------
 */
SYSCALL releaseall (int numlocks, int args)
{
	STATWORD ps;    
	struct pentry *pptr;
	struct lentry *lptr;
	int i, *lockptr, lock;
	int retcode = OK;
    pptr = &proctab[currpid];
	disable(ps);    
    // Loop through all locks to release them
	for(i=0; i< numlocks; i++){
		lockptr = &args + i;
		lock = *lockptr;
        lptr= &locks[lock];
		if (isbadlock(lock) || lptr->lstate==LFREE || !(lptr->lock_acquired[currpid]) ) 
			retcode = SYSERR;		
		else
            release_lock(lock, lptr, pptr);       
	}
	priority_inherit(pptr);
	restore(ps);
	return retcode;
}

void release_lock(int lock, struct lentry *lptr, struct pentry *pptr){
    // decrement the active readers and update lock acquired
    if(lptr->ltype == READ)
        lptr->l_active_readers--;
    lptr->lock_acquired[currpid] = 0;
    pptr->plocksacquired[lock] = 0;
    
    /* check for the first reader or writer */
    if(lptr->ltype == WRITE || ((lptr->ltype == READ) && (lptr->l_active_readers == 0)) ){
        lptr->lcnt++;				
        /* acquire lock if free and get the max priority of the process in the */
        if( nonempty(lptr->lqhead) ){	
            int max_prio_read = highest_priority_process_with_same_type(lock,READ);
            int max_prio_write = highest_priority_process_with_same_type(lock,WRITE);
            
            if(q[max_prio_read].qkey > q[max_prio_write].qkey){
                while(max_prio_read > -1 && (q[max_prio_read].qkey > q[max_prio_write].qkey) ){
                    read_write_access_control(lptr, max_prio_read, lock,READ);
                    max_prio_read = highest_priority_process_with_same_type(lock,READ);
                }
            }
            else if(q[max_prio_read].qkey < q[max_prio_write].qkey ||
                        ((q[max_prio_read].qlockwaittime >= q[max_prio_write].qlockwaittime) &&
                         (q[max_prio_read].qlockwaittime - q[max_prio_write].qlockwaittime <= 500 ))){
                read_write_access_control(lptr, max_prio_write, lock,WRITE);
            }
            else{
                read_write_access_control(lptr, max_prio_read, lock,READ);
            }
            resched();
        }
    }
}

/* Marking lock is acquired by the process, if READ - incrementing the number of readers, removing the process from the queue,
   updating the priotity and finally clearing the plock as the process now acquired the lock */
void read_write_access_control(struct lentry *lptr, int max_prio, int lock, int type){
    lptr->lock_acquired[max_prio] = 1;
    proctab[max_prio].plocksacquired[lock] = 1;
    if( type = READ)
        lptr->l_active_readers++;
    lptr->ltype = type;
    ready(dequeue(max_prio), RESCHNO);
    lptr->lprio = waiting_process_with_highest_priority(lptr);
    proctab[max_prio].plock = -1;
}

int highest_priority_process_with_same_type(int lock, int ltype){
	struct lentry *lptr = &locks[lock];
	int best_proc = -1;
	if(!isempty(lptr->lqhead)){
        int prev = q[lptr->lqtail].qprev;
        while(prev != lptr->lqhead)
        {
            if(q[prev].qlocktype == ltype){
                best_proc = prev;
                break;
            }
            prev = q[prev].qprev;
        }
    }
    return best_proc;
}

void priority_inherit(struct pentry * pptr){
	struct lentry *bestlptr = NULL;
	struct lentry *lptr;
	int i, numlocks = 0;
 
	for(i =0; i < NLOCKS; i++){           
		lptr = &locks[i];            
		if(pptr->plocksacquired[i]){
			numlocks++;
			if(numlocks == 1 || (bestlptr != NULL && schedprio(pptr) > bestlptr->lprio ) )
				bestlptr = lptr;
		}
	}
    if(!numlocks)		
	    pptr->pinh = 0;
    else{
        pptr->pinh = bestlptr->lprio;
        pptr->pinh = (pptr->pprio > pptr->pinh) ? 0 : bestlptr->lprio;
    }
	if(pptr->plock != -1){
		lptr = &locks[pptr->plock];
		lptr->lprio = waiting_process_with_highest_priority(lptr);

		for(i=0; i< NPROC; i++){
			if(lptr->lock_acquired[i])
				priority_inherit(&proctab[i]);	
		}
	}
}

