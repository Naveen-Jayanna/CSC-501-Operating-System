/* paging_helper.c */
#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

/*-------------------------------------------------------------------------
 * print_table - print frame table
 *-------------------------------------------------------------------------
 */
void print_table(){
	int i;
	kprintf("\nPrinting frame table\n");
	for(i=0; i < NFRAMES; i++){
		// kprintf("\n\n\nTesting ---------------------------------------- paging_helper - print_table()  frame_tab[i].fr_status = %d\n\n\n", frame_tab[i].fr_status);
		if(frame_tab[i].fr_status){
			kprintf("i = %d, status = %d, type = %d, pid = %d, vpno = %d\n",i,frame_tab[i].fr_status, frame_tab[i].fr_type, frame_tab[i].fr_pid, frame_tab[i].fr_vpno);
		}
	}
	kprintf("\n");
}

/*-------------------------------------------------------------------------
 * allocate_page_dir - allocate a page directory
 *-------------------------------------------------------------------------
 */
int allocate_page_dir(int pid, int *pdfrm){
	// kprintf("\n\n\nTesting ---------------------------------------- paging_helper - allocate_page_dir(), pdfrm = %d\n\n\n", *pdfrm);
	if( get_frm(pdfrm) == SYSERR)
		return SYSERR;

	frame_tab[*pdfrm].fr_type = FR_DIR;
	frame_tab[*pdfrm].fr_status = FRM_MAPPED;
	frame_tab[*pdfrm].fr_pid = pid;
	return OK;
}

/*-------------------------------------------------------------------------
 * initialize_page_dir - initialize the page directory
 *-------------------------------------------------------------------------
 */
void initialize_page_dir(int pdfrm){
	pd_t *pde;
	int i;
	pde = (pd_t *)((FRAME0 + pdfrm) * NBPG);
	// kprintf("\n\n\nTesting ---------------------------------------- paging_helper - 1. initialize_page_dir() %d\n\n\n", pde);
        for(i=0; i < NPDE; i++){
			if(i < 4){
				pde->pd_pres = 1;
				pde->pd_base = FRAME0 + i;
			}
			else{
				pde->pd_pres = 0;
			}

			// kprintf("\n\n\nTesting ---------------------------------------- paging_helper -2. initialize_page_dir()\n\n\n");
			// Initialize the values
			pde->pd_pcd = 0;
			pde->pd_fmb = 0;
			pde->pd_user = 0;
			pde->pd_acc = 0;
			pde->pd_pwt = 0;
			pde->pd_avail = 0;
			pde->pd_mbz = 0;
			pde->pd_global = 0;
			pde->pd_write = 1;
			pde++; 
        }

}


/*-------------------------------------------------------------------------
 *  * allocate_page_table - allocate a page table
 *   *-------------------------------------------------------------------------
 *    */
int allocate_page_table(int pid, int *ptfrm){
        if( get_frm(ptfrm) == SYSERR)
                return SYSERR;

	// kprintf("\n\n\nTesting ---------------------------------------- paging_helper - allocate_page_table()\n\n\n");
        frame_tab[*ptfrm].fr_type = FR_TBL;
        frame_tab[*ptfrm].fr_status = FRM_MAPPED;
        frame_tab[*ptfrm].fr_pid = pid;

        return OK;
}

/*-------------------------------------------------------------------------
 *  * initialize_page_table - initialize page table
 *   *-------------------------------------------------------------------------
 *    */
void initialize_page_table(int ptfrm){
        int i;
        pt_t *pte;

        pte = (pt_t *)((FRAME0 + ptfrm) * NBPG);
	// kprintf("\n\n\nTesting ---------------------------------------- paging_helper - initialize_page_table()\n\n\n");

        for(i=0; i < NPTE; i++){
			pte->pt_avail = 0;
			pte->pt_user = 0;
			pte->pt_pcd = 0;
			pte->pt_pres = 0;
			pte->pt_dirty = 0;
			pte->pt_global = 0;
			pte->pt_pwt = 0;
			pte->pt_acc = 0;
			pte->pt_mbz = 0;
			pte->pt_write = 1;
			pte++;
        }

}
/*-----------------------------------------------------------------------------------------
 * remove_page_table_entry - remove page table entry from page table and write to BS based on write type
 *-----------------------------------------------------------------------------------------
 */
void remove_page_table_entry(int i){
	
	int pid, vpno, write_type;
	unsigned int p,q,store,pageth;
	pd_t *pde;
	pt_t *pte;
	unsigned long vaddr, pdbr, pt;
	virt_addr_t virtaddr;

	pid = frame_tab[i].fr_pid;
	vpno = frame_tab[i].fr_vpno;
	write_type = frame_tab[i].fr_wtype;
	
	vaddr = vpno * NBPG;	
	get_virtual_address(vaddr, &virtaddr);        
        p = virtaddr.pd_offset;
        q = virtaddr.pt_offset; 

        pdbr = proctab[pid].pdbr;
        pt = pdbr + sizeof(pd_t) * p;   
	pde = (pd_t *) pt;                                     
	pte = (pt_t *) (pde->pd_base * NBPG + sizeof(pt_t) * q );

       	pte->pt_pres = 0;

      	if(pid == currpid)
		// kprintf("\n\n\nTesting ---------------------------------------- paging_helper - remove_page_table_entry() ----- pid == currpid\n\n\n");
			;            

        frame_tab[pde->pd_base].fr_refcnt--;    
	if(!frame_tab[pde->pd_base].fr_refcnt)
 	   pde->pd_pres = 0;

        if(pte->pt_dirty){
                if(bsm_lookup(pid, vaddr, &store, &pageth) == SYSERR){
		// kprintf("\n\n\nTesting ---------------------------------------- paging_helper - remove_page_table_entry() ----- write_type = %d\n\n\n", write_type);
			if(write_type != WR_FORCED){
				kprintf("Lookup failed in the backing store. Process %d killed\n",pid);
	            kill(pid);
			}
                }
              	write_bs((char *)(pte->pt_base * NBPG), store, pageth);
        }
}

/*------------------------------------------------------------------------------
 *  * get_virtual_address - convert virtual address from unsigned long to virt_addr_t
 *  *---------------------------------------------------------------------------
 *  */

int get_virtual_address(unsigned long vaddr, virt_addr_t *virtaddr){
	virtaddr->pg_offset = vaddr & 4095UL;
	virtaddr->pt_offset = (vaddr & 4190208UL)>>12;
	virtaddr->pd_offset = (vaddr & 4290772992UL)>>22;
	return OK;
}
/*--------------------------------------------------------------------------
 *  * get_virtual_page_number - extract virtual page number from virtual address 
 *  *-----------------------------------------------------------------------
 *  */

int get_virtual_page_number(virt_addr_t virtaddr, int *vpno){
	*vpno = (((int)virtaddr.pd_offset) << 10) | ((int)virtaddr.pt_offset);
	return OK;
}

