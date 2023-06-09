/* resched.c  -  resched */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <paging.h>

unsigned long currSP;	/* REAL sp of current process */

/*------------------------------------------------------------------------
 * resched  --  reschedule processor to highest priority ready process
 *
 * Notes:	Upon entry, currpid gives current process id.
 *		Proctab[currpid].pstate gives correct NEXT state for
 *			current process if other than PRREADY.
 *------------------------------------------------------------------------
 */
int	resched()
{
	STATWORD		PS;
	register struct	pentry	*optr;	/* pointer to old process entry */
	register struct	pentry	*nptr;	/* pointer to new process entry */
	register int i;
	int vpno;
	unsigned int prev_p,prev_q,store,pageth;
	unsigned long vaddr, pdbr, pt;
	virt_addr_t virtaddr;
	pd_t *pde;
	pt_t *pte;
	disable(PS);
	/* no switch needed if current process priority higher than next*/

	if ( ( (optr= &proctab[currpid])->pstate == PRCURR) &&
	   (lastkey(rdytail)<optr->pprio)) {
		restore(PS);
		return(OK);
	}
	
#ifdef STKCHK
	/* make sure current stack has room for ctsw */
	asm("movl	%esp, currSP");
	if (currSP - optr->plimit < 48) {
		kprintf("Bad SP current process, pid=%d (%s), lim=0x%lx, currently 0x%lx\n",
			currpid, optr->pname,
			(unsigned long) optr->plimit,
			(unsigned long) currSP);
		panic("current process stack overflow");
	}
#endif	

	/* force context switch */

	if (optr->pstate == PRCURR) {
		optr->pstate = PRREADY;
		insert(currpid,rdyhead,optr->pprio);
	}

	/*the previous process pages are written on shared BS for the new process to access */
	for(i=0; i < NFRAMES; i++){
		if(frame_tab[i].fr_pid == currpid && frame_tab[i].fr_type == FR_PAGE){    
			vpno = frame_tab[i].fr_vpno;
			vaddr = vpno * NBPG;	
			get_virtual_address(vaddr, &virtaddr);        
			prev_p = virtaddr.pd_offset;
			prev_q = virtaddr.pt_offset; 

 			// kprintf("\n\n\nTesting ---------------------------------------- pfint.c - pfint() ------ prev_p = %d, prev_q = %d \n\n\n", prev_p, prev_q);

			pdbr = proctab[currpid].pdbr;
			pt = pdbr + sizeof(pd_t) * prev_p;   
			pde = (pd_t *) pt;
			pte = (pt_t *) (pde->pd_base * NBPG + sizeof(pt_t) * prev_q );       

			// check for dirty page and write into BS
			if(pte->pt_dirty){
				if(bsm_lookup(currpid, vaddr, &store, &pageth) == SYSERR){				
					kprintf("Backing Store lookup failed. The process with pid = %d is killed.\n",currpid);
					kill(currpid);
				}
				if(bsm_tab[store].bs_shared_priv == BSM_SHARED)
					write_bs((char *)(pte->pt_base * NBPG), store, pageth);
			}
		}
    }  

	/* remove highest priority process at end of ready list */

	nptr = &proctab[ (currpid = getlast(rdytail)) ];
	nptr->pstate = PRCURR;		/* mark it currently running	*/

	
	// For the new process, latest pages are read
	for(i=0; i <NFRAMES; i++){
		if(frame_tab[i].fr_pid == currpid && frame_tab[i].fr_type == FR_PAGE){ 
			vpno = frame_tab[i].fr_vpno;
            vaddr = vpno * NBPG;
			
			if(bsm_lookup(currpid, vaddr, &store, &pageth) == SYSERR){
				kprintf("Backing Store lookup failed! Killing process %d!\n",currpid);
				kill(currpid);
			} 
			if(bsm_tab[store].bs_shared_priv == BSM_SHARED)
				read_bs((char*)((FRAME0 + i)*NBPG), store, pageth);
		}
	}


#ifdef notdef
#ifdef	STKCHK
	if ( *( (int *)nptr->pbase  ) != MAGIC ) {
		kprintf("Bad magic pid=%d value=0x%lx, at 0x%lx\n",
			currpid,
			(unsigned long) *( (int *)nptr->pbase ),
			(unsigned long) nptr->pbase);
		panic("stack corrupted");
	}
	/*
	 * need ~16 longs of stack space below, so include that in check
	 *	below.
	 */
	if (nptr->pesp - nptr->plimit < 48) {
		kprintf("Bad SP pid=%d (%s), lim=0x%lx will be 0x%lx\n",
			currpid, nptr->pname,
			(unsigned long) nptr->plimit,
			(unsigned long) nptr->pesp);
		panic("stack overflow");
	}
#endif	/* STKCHK */
#endif	/* notdef */
#ifdef	RTCLOCK
	preempt = QUANTUM;		/* reset preemption counter	*/
#endif
#ifdef	DEBUG
	PrintSaved(nptr);
#endif
	// placing before the context switch as this subroutine ends in the context of a new process
	write_cr3(nptr->pdbr);
	ctxsw(&optr->pesp, optr->pirmask, &nptr->pesp, nptr->pirmask);

#ifdef	DEBUG
	PrintSaved(nptr);
#endif
	
	/* The OLD process returns here when resumed. */
	restore(PS);
	return OK;
}



#ifdef DEBUG
/* passed the pointer to the regs in the process entry */
PrintSaved(ptr)
    struct pentry *ptr;
{
    unsigned int i;

    if (ptr->pname[0] != 'm') return;
    
    kprintf("\nSaved context listing for process '%s'\n",ptr->pname);
    for (i=0; i<8; ++i) {
	kprintf("     D%d: 0x%08lx	",i,(unsigned long) ptr->pregs[i]);
	kprintf("A%d: 0x%08lx\n",i,(unsigned long) ptr->pregs[i+8]);
    }
    kprintf("         PC: 0x%lx",(unsigned long) ptr->pregs[PC]);
    kprintf("  SP: 0x%lx",(unsigned long) ptr->pregs[SSP]);
    kprintf("  PS: 0x%lx\n",(unsigned long) ptr->pregs[PS]);
}
#endif


