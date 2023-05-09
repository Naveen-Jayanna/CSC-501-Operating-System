/* lqueueinsert.c  -  lqueueinsert */

#include <conf.h>
#include <kernel.h>
#include <q.h>
#include <lock.h>

extern unsigned long ctr1000;
/*----------------------------------------------------------------------------------
 * lqueueinsert.c  --  insert a process into the queue based on priority(key value)
 *----------------------------------------------------------------------------------
 */
int lqueueinsert(int proc, int head, int wprio, int ltype)
{
	int	next, prev;
    // Traverse the queue until we find the correct position for the new process
	next = q[head].qnext;
	while (q[next].qkey < wprio){
		next = q[next].qnext;
    }
    // Update the links to insert the new process in the queue
	q[proc].qnext = next;
	q[proc].qprev = prev = q[next].qprev;
	q[proc].qkey  = wprio;
	q[proc].qlocktype = ltype;
	q[proc].qlockwaittime = ctr1000;
	q[prev].qnext = proc;
	q[next].qprev = proc;
	
	return(OK);
}
