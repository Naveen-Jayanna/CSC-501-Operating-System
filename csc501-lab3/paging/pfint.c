/* pfint.c - pfint */

#include <conf.h>
#include <kernel.h>
#include <paging.h>
#include <proc.h>

/*-------------------------------------------------------------------------
 * pfint - paging fault ISR
 *-------------------------------------------------------------------------
 */
SYSCALL pfint()
{
	STATWORD ps;
	disable(ps);
	unsigned long fault_address, pdbr, pt, destination_address;
	int vpno, ptfrm, store, pageth, freefrm, dst, i;
	virt_addr_t faultaddr;	
	unsigned int p,q;
	pd_t *pde;
	pt_t *pte;

	/* read the faulted address and get virtual address and page number */
	fault_address = read_cr2();
	get_virtual_address(fault_address, &faultaddr);
	get_virtual_page_number(faultaddr, &vpno);
	pdbr = proctab[currpid].pdbr;

 	// kprintf("\n\n\nTesting ---------------------------------------- pfint.c - pfint() ------ pdbr = %d\n\n\n", pdbr);

	p = faultaddr.pd_offset;
	q = faultaddr.pt_offset;
	
 	// kprintf("\n\n\nTesting ---------------------------------------- pfint.c - pfint() ------ p = %d, q = %d \n\n\n", p, q);

	pt = pdbr + (sizeof(pd_t)) * p;	
 	pde = (pd_t *) pt;
	
	/* if the required page table is not present, update the page directory with new values of the page table */
	if(!pde->pd_pres){
		allocate_page_table(currpid, &ptfrm);
		initialize_page_table(ptfrm);
		
		pde->pd_pres = 1;
		pde->pd_write = 1;
		pde->pd_user = 0;
		pde->pd_pwt = 0;
		pde->pd_pcd = 0;
		pde->pd_acc = 0;
		pde->pd_mbz = 0;
		pde->pd_fmb = 0;
		pde->pd_global = 0;
		pde->pd_avail = 0;
		pde->pd_base = FRAME0 + ptfrm;
	}

	/* look up the faulted page */ 
	bsm_lookup(currpid, fault_address, &store, &pageth);

	frame_tab[pde->pd_base - FRAME0].fr_refcnt++;
	get_frm(&freefrm);
 	// kprintf("\n\n\nTesting ---------------------------------------- pfint.c - pfint() ------ freefrm = %d \n\n\n",freefrm);
	destination_address = (FRAME0 + freefrm) * NBPG;
	read_bs((char *)destination_address , store, pageth);

	/* update the frame table with the new values */
	frame_tab[freefrm].fr_status = FRM_MAPPED;
	frame_tab[freefrm].fr_pid = currpid;
	frame_tab[freefrm].fr_type = FR_PAGE;
	frame_tab[freefrm].fr_vpno = vpno;
	frame_tab[freefrm].fr_wtype = WR_BASIC;
	
	/* set the fr_sc value for SC page replacement policy */
	frame_tab[freefrm].fr_sc = 1;	
	sc_queue_pages++;

	/* first frame into the SC circular queue */
	if(sc_queue_pages == 1)	
		sc_index = freefrm;

	/* append the free frame for FIFO page replacement policy */
	append_fifo(&fifohead, freefrm);

	/* update thbe page table */
	pte = (pt_t *) ((pde->pd_base * NBPG) + (sizeof(pt_t) * q));

	pte->pt_pres = 1;
	pte->pt_base = FRAME0 + freefrm;
	restore(ps);
  	return OK;
}


