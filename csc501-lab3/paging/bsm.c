/* bsm.c - manage the backing store mapping*/

#include <conf.h>
#include <kernel.h>
#include <paging.h>
#include <proc.h>

/*-------------------------------------------------------------------------
 * init_bsm- initialize bsm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL init_bsm()
{
	STATWORD ps; 
	disable(ps);
	
	int i,j;

	for(i=0; i < NBS; i++){
		bsm_tab[i].bs_status =  BSM_UNMAPPED;

		for(j=0; j < NPROC; j++){		
			bsm_tab[i].bs_pid[j] = 0;
        	bsm_tab[i].bs_vpno[j] = -1;
		}

        bsm_tab[i].bs_npages = -1;
		bsm_tab[i].bs_shared_priv = BSM_SHARED;
		bsm_tab[i].bs_total_proc = 0;
		
	}
	restore(ps);
	return OK;	 
}

/*-------------------------------------------------------------------------
 * get_bsm - get a free entry from bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL get_bsm(int* avail)
{
	STATWORD ps; 
	disable(ps);
	
	int i;
	for(i=0; i< NBS; i++){
		if(bsm_tab[i].bs_status == BSM_UNMAPPED){	
			bsm_tab[i].bs_status = BSM_MAPPED;
			*avail = i;
			restore(ps);
			return OK;
		}
	}
	restore(ps);
	return SYSERR;
}


/*-------------------------------------------------------------------------
 * free_bsm - free an entry from bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL free_bsm(int i)
{
	STATWORD ps; 
	disable(ps);

	if(isbadbs(i)){ 
		restore(ps);
		return SYSERR;
	}
	
	bsm_tab[i].bs_status = BSM_UNMAPPED;
	if(bsm_tab[i].bs_shared_priv == BSM_PRIV){	
		bsm_tab[i].bs_shared_priv = BSM_SHARED;
    }
	restore(ps);
	return OK;
}

/*-------------------------------------------------------------------------
 * bsm_lookup - lookup bsm_tab and find the corresponding entry
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_lookup(int pid, long vaddr, int* store, int* pageth)
{
	STATWORD ps;
	disable(ps);

	/* sanity check */
	if(isbadpid(pid)){
		restore(ps);
		return SYSERR;	        
	}

	virt_addr_t virtaddr;
	int vpno, i;
	get_virtual_address(vaddr, &virtaddr);
	get_virtual_page_number(virtaddr,&vpno);
	
	for(i=0; i < NBS; i++){
 		// kprintf("\n\n\nTesting ---------------------------------------- bsm.c - bsm_lookup() ------ pid = %d, bs_vpno = %d \n\n\n", pid, bsm_tab[i].bs_vpno[pid]);
		if(bsm_tab[i].bs_pid[pid] && bsm_tab[i].bs_vpno[pid] != -1 && 
			vpno >= bsm_tab[i].bs_vpno[pid] && vpno < bsm_tab[i].bs_vpno[pid] + bsm_tab[i].bs_npages){
			*store = i;
			*pageth = vpno - bsm_tab[i].bs_vpno[pid]; 
			restore(ps); 
			return OK;
		}
	}
	restore(ps);
	return SYSERR;	
}


/*-------------------------------------------------------------------------
 * bsm_map - add an mapping into bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_map(int pid, int vpno, int source, int npages)
{
	STATWORD ps;
	disable(ps);

	/* sanity check */
	if(isbadpid(pid) || isbadvpno(vpno) || isbadbs(source) || npages <= 0 || npages > NPBS || bsm_tab[source].bs_shared_priv == BSM_PRIV){
		restore(ps);
		return SYSERR;
    }
		
	bsm_tab[source].bs_pid[pid] = 1;
	bsm_tab[source].bs_total_proc += 1;
    bsm_tab[source].bs_vpno[pid] = vpno;

	if(bsm_tab[source].bs_npages == -1){
	    bsm_tab[source].bs_npages = npages;
    }
	restore(ps);
	return OK;		
}



/*-------------------------------------------------------------------------
 * bsm_unmap - delete an mapping from bsm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_unmap(int pid, int vpno, int flag)
{
	STATWORD ps;
    disable(ps);
	
	/* sanity check */
	if(isbadpid(pid) || isbadvpno(vpno)){
		restore(ps);
		return SYSERR;
	}
	int i,j;
	// unsigned long pdbr;
	// pdbr = proctab[pid].pdbr;
 	// kprintf("\n\n\nTesting ---------------------------------------- bsm.c - bsm_unmap() ------ pdbr = %d\n\n\n", pdbr);

	for(i=0;i<NBS; i++){
 		 // kprintf("\n\n\nTesting ---------------------------------------- bsm.c - bsm_unmap() ------ pid = %d, bs_vpno = %d \n\n\n", pid, bsm_tab[i].bs_vpno[pid]);

        if(bsm_tab[i].bs_pid[pid] && bsm_tab[i].bs_vpno[pid] != -1 &&
            vpno >= bsm_tab[i].bs_vpno[pid] && vpno < bsm_tab[i].bs_vpno[pid] + bsm_tab[i].bs_npages){
			
		   	for(j=0; j < NFRAMES; j++){
                if(frame_tab[j].fr_pid == pid && frame_tab[j].fr_type == FR_PAGE){
					free_frm(j);
        	    }
			}	
			bsm_tab[i].bs_pid[pid] = 0;
			bsm_tab[i].bs_vpno[pid] = -1;
			
			bsm_tab[i].bs_total_proc = bsm_tab[i].bs_total_proc - 1;

			if(!bsm_tab[i].bs_total_proc){
				bsm_tab[i].bs_npages = -1;
				free_bsm(i);
			}
			restore(ps);
			return OK;
		}
	}
	restore(ps);
	return SYSERR;	
}
