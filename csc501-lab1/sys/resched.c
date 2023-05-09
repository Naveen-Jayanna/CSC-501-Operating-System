/* resched.c  -  resched */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include "lab1.h"

unsigned long currSP;	/* REAL sp of current process */
extern int ctxsw(int, int, int, int);

int current_priority = 0;
int epoch = 0;
int maximumgoodness();
int maximumgoodnessproc();

int maximumgoodness()
{
	int curr_proc, maxgoodness;
	maxgoodness = -1;
	for(curr_proc = q[rdyhead].qnext; curr_proc != rdytail; curr_proc = q[curr_proc].qnext){
		if(proctab[curr_proc].goodness > maxgoodness)
			maxgoodness = proctab[curr_proc].goodness;
	}
	return maxgoodness;
}


int maximumgoodnessproc()
{
	int curr_proc, maxgoodness, maxgoodnessproc;
	maxgoodness = -1;
	maxgoodnessproc = 0;
	for(curr_proc = q[rdyhead].qnext; curr_proc != rdytail; curr_proc = q[curr_proc].qnext){
		if(proctab[curr_proc].goodness > maxgoodness){
			maxgoodness = proctab[curr_proc].goodness;
			maxgoodnessproc = curr_proc;
		}
	}
	return maxgoodnessproc;
}

void agingsched()
{
	/* Implementation of the Aging Scheduler - gradually increase the priority of
		the processes waiting to be executed every time resched is called*/

	/* Increasing the priority of each process in the ready queue by 1(aging)*/
	int current_process = q[rdyhead].qnext;
	while(current_process != rdytail){
		q[current_process].qkey += 1;
		proctab[current_process].pprio += 1;
		current_process = q[current_process].qnext;
	}
}

int linuxsched()
{
	int i;
	register struct pentry  *optr;  /* pointer to old process entry */
	register struct pentry  *nptr;  /* pointer to new process entry */
	optr= &proctab[currpid];

	int curr = q[rdyhead].qnext;
	while(curr != rdytail){
		q[curr].qkey = proctab[curr].goodness;
		curr = q[curr].qnext;
	}

	// id the current process is not a NULL process
	// calculate the quantum used based on the current unused_quantum and the preempt value(10 ticks, default)
	if(currpid != NULLPROC){
		int quantum_used = optr->unused_quantum - preempt;
		epoch -= quantum_used;
		optr->goodness = optr->goodness - quantum_used;
		optr->unused_quantum = preempt;
		
		if(optr->unused_quantum == 0){
			optr->goodness = 0;
		}
	}

	//for every new epoch find the maximum goodness of all the processes isempty(rdyhead)
	//starting a new epoch only of the maxgoodness <= 0 and if the old process is not a current process or if its a current process then its goodness is 0
	if(maximumgoodness()<=0 && (optr->pstate != PRCURR || (optr->pstate == PRCURR && optr->goodness == 0))){
		// kprintf("\n new epoch init: processes gathered for the current epoch are:\n"); 
		epoch=0;
		proctab[0].goodness = 0; // godness value of the NULL process
		for(i=1; i<NPROC; i++){  // as NULL process id is 0, skipping it, run the loop for all the other processes
			if(proctab[i].pstate != PRFREE){			// if the process is not in FREE state, calculate the quantum, unused_quantum, and goodness value.
				// kprintf("\nprocess = %d\n", i);
				if(proctab[i].unused_quantum == 0 || proctab[i].unused_quantum == proctab[i].quantum)
					proctab[i].quantum = proctab[i].pprio;
				else
					proctab[i].quantum = (0.5 * proctab[i].unused_quantum) + proctab[i].pprio;	
				proctab[i].unused_quantum = proctab[i].quantum;
				proctab[i].goodness = proctab[i].unused_quantum + proctab[i].pprio;
				epoch += proctab[i].quantum;			// Finally, update the epoch
			}
		}
	}

	// kprintf("\n Information of the process before swapping.");
	// kprintf("\n Current pid: %d\t quantum allocated = %d\t unused_quantum = %d\n", currpid, optr->quantum, optr->unused_quantum);

	/* Context switch is not needed if the current process has a greater goodnes than the next process*/
	if (( optr->pstate == PRCURR) && (maximumgoodness() < optr->goodness)){
		preempt = optr->unused_quantum;       
		// kprintf("\nNo context switch needed");
		return(OK);
	}

	/* else: forced context switch and save the goodness value as the priority*/
	if (optr->pstate == PRCURR) {
			optr->pstate = PRREADY;
			insert(currpid,rdyhead,optr->goodness);
	}

	if(maximumgoodness() > 0 && maximumgoodnessproc() > 0 ){
		currpid = dequeue(maximumgoodnessproc());		/* remove highest goodness process from the ready list */
		nptr = &proctab[currpid];

	#ifdef  RTCLOCK
		preempt = nptr->unused_quantum;              /* preempt value is changed to unused quantum of the new process */
	#endif

	}
	else{
		nptr = &proctab[(currpid = dequeue(0))];             /* if no processes are running, the NULL process is scheduled */
		#ifdef  RTCLOCK
			preempt = QUANTUM;              /* reset preemption counter     */
		#endif

	}

	nptr->pstate = PRCURR;          /* mark it currently running    */
	// kprintf("\n Current pid: %d\t quantum allocated = %d\t unused_quantum = %d\n", currpid, optr->quantum, optr->unused_quantum);
	// kprintf("\n Information of the process after swapping.");
	ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);

	/* The OLD process returns here when resumed. */
	return OK;
}

int defaultsched()
{
	/* Use the default scheduler of the XINU process */
	register struct	pentry	*optr;	/* pointer to old process entry */
	register struct	pentry	*nptr;	/* pointer to new process entry */

	/* no switch needed if current process priority higher than next*/

	if ( ( (optr= &proctab[currpid])->pstate == PRCURR) &&
	(lastkey(rdytail)<optr->pprio)) {
		return(OK);
	}
	
	/* force context switch */

	if (optr->pstate == PRCURR) {
		optr->pstate = PRREADY;
		insert(currpid,rdyhead,optr->pprio);
	}

	/* remove highest priority process at end of ready list */

	nptr = &proctab[ (currpid = getlast(rdytail)) ];
	nptr->pstate = PRCURR;		/* mark it currently running	*/
	#ifdef	RTCLOCK
		preempt = QUANTUM;		/* reset preemption counter	*/
	#endif
	
	ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
	
	/* The OLD process returns here when resumed. */
	return OK;
}

/*-----------------------------------------------------------------------
 * resched  --  reschedule processor to highest priority ready process
 *
 * Notes:	Upon entry, currpid gives current process id.
 *		Proctab[currpid].pstate gives correct NEXT state for
 *			current process if other than PRREADY.
 *------------------------------------------------------------------------
 */
int resched()
{

	if(getschedclass() == LINUXSCHED)
		return linuxsched();
	if(getschedclass() == AGESCHED)
		agingsched();
	return defaultsched();
}