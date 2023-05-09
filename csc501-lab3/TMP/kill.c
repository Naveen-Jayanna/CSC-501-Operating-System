/* kill.c - kill */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <q.h>
#include <stdio.h>
#include <paging.h>

/*------------------------------------------------------------------------
 * kill  --  kill a process and remove it from the system
 *------------------------------------------------------------------------
 */
SYSCALL kill(int pid)
{
	STATWORD ps;    
	struct	pentry	*pptr;		/* points to proc. table for pid*/
	int	i, j, pdfrm, dev;
	long vaddr;

	disable(ps);
	if (isbadpid(pid) || (pptr= &proctab[pid])->pstate==PRFREE) {
		restore(ps);
		return(SYSERR);
	}
	if (--numproc == 0)
		xdone();

	dev = pptr->pdevs[0];
	if (! isbaddev(dev) )
		close(dev);
	dev = pptr->pdevs[1];
	if (! isbaddev(dev) )
		close(dev);
	dev = pptr->ppagedev;
	if (! isbaddev(dev) )
		close(dev);
	
	send(pptr->pnxtkin, pid);
	freestk(pptr->pbase, pptr->pstklen);

	for(i=0; i < NFRAMES; i++){
		if(frame_tab[i].fr_pid == pid && frame_tab[i].fr_type == FR_DIR){
			pdfrm = i;
		}
		else if(frame_tab[i].fr_pid == pid && frame_tab[i].fr_type == FR_PAGE){
			frame_tab[i].fr_wtype = WR_FORCED;
			free_frm(i);
		}
	}
	for(i=0; i < NBS; i++){
		if(bsm_tab[i].bs_pid[pid]){
			bsm_unmap(pid, bsm_tab[i].bs_vpno[pid], 0);
	}

	}
	free_frm(pdfrm);

	switch (pptr->pstate) {

	case PRCURR:	pptr->pstate = PRFREE;	/* suicide */
			resched();

	case PRWAIT:	semaph[pptr->psem].semcnt++;

	case PRREADY:	dequeue(pid);
			pptr->pstate = PRFREE;
			break;

	case PRSLEEP:
	case PRTRECV:	unsleep(pid);
						/* fall through	*/
	default:	pptr->pstate = PRFREE;
	}
	restore(ps);
	return(OK);
}
