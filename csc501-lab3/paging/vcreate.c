/* vcreate.c - vcreate */
    
#include <conf.h>
#include <i386.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <paging.h>

/*
static unsigned long esp;
*/

LOCAL	newpid();
/*------------------------------------------------------------------------
 *  create  -  create a process to start running a procedure
 *------------------------------------------------------------------------
 */
SYSCALL vcreate(procaddr,ssize,hsize,priority,name,nargs,args)
	int	*procaddr;		/* procedure address		*/
	int	ssize;			/* stack size in words		*/
	int	hsize;			/* virtual heap size in pages	*/
	int	priority;		/* process priority > 0		*/
	char	*name;			/* name (for debugging)		*/
	int	nargs;			/* number of args that follow	*/
	long	args;			/* arguments (treated like an	*/
					/* array in the code)		*/
{
	STATWORD ps;    
	int	pid;					/* process id	*/
	int bs;						/* backing store	*/
	unsigned long unmap_vpno;	/* first unmapped virtual page for the current process	*/
	struct mblock *mptr;		/* pointer to free memory block */
	
	disable(ps);

	/* raising an error if the heap size is not between 0 and 16 BS */
	if (hsize <= 0 || hsize > NBS*NPBS) {
		restore(ps);
		return(SYSERR);
	}

	pid = create(procaddr,ssize,priority,name,nargs,args);
	
	unmap_vpno = FVPP;
	mptr = &proctab[pid].vmemlist;
	
	/* backing store allocation and virtual heap mapping for process */ 
	while(hsize >0){
		// raise an error if the BS is not available
		if(get_bsm(&bs) == SYSERR){
			restore(ps);
			return SYSERR;
		}

		mptr->mnext = (struct mblock*)  (BACKING_STORE_BASE + bs * BACKING_STORE_UNIT_SIZE);

		/* check if this is the last backing store allocated to the process */
		if(hsize > NPBS){
			bsm_map(pid, unmap_vpno, bs, NPBS);	
			unmap_vpno += NPBS;	
			hsize -= NPBS;
			mptr->mnext->mlen = NPBS * NBPG;
			mptr = mptr->mnext;
		}
		else{
			bsm_map(pid, unmap_vpno, bs, hsize);
			unmap_vpno += hsize;
			mptr->mnext->mlen = hsize * NBPG;
			mptr->mnext->mnext = NULL;
			hsize = 0;
		}

		/* the backing store is set to be private to the process */
		bsm_tab[bs].bs_shared_priv = BSM_PRIV;
	}
	

	restore(ps);
	return(pid);
	
}

/*------------------------------------------------------------------------
 * newpid  --  obtain a new (free) process id
 *------------------------------------------------------------------------
 */
LOCAL	newpid()
{
	int	pid;			/* process id to return		*/
	int	i;

	for (i=0 ; i<NPROC ; i++) {	/* check all NPROC slots	*/
		if ( (pid=nextproc--) <= 0)
			nextproc = NPROC-1;
		if (proctab[pid].pstate == PRFREE)
			return(pid);
	}
	return(SYSERR);
}
