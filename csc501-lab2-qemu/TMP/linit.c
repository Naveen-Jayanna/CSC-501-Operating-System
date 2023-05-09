/* linit.c - linit */

#include <stdio.h>
#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <lock.h>
#include <q.h>

/*------------------------------------------------------------------------
 * linit - initilize the lentry values
 *------------------------------------------------------------------------
 */
void linit(){
    int i;
    struct	lentry	*lockptr;

    /* for every lock, mark state as free, create a new queue and update the head and tail  */
    for (i=0 ; i<NLOCKS ; i++) {
        lockptr = &locks[i];
        lockptr->lstate = LFREE;
        lockptr->lqhead = newqueue();
        lockptr->lqtail = lockptr->lqhead + 1;
    }
}
