#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

/* release the backing store with ID bs_id */
SYSCALL release_bs(bsd_t bs_id) {
	STATWORD ps; 
	disable(ps);

	int i;
 	// kprintf("\n\n\nTesting ---------------------------------------- release_bs.c - release_bs() ------ bs_id = %d, bsm_tab[bs_id].bs_total_proc = %d \n\n\n", bs_id, bsm_tab[bs_id].bs_total_proc);

	/* sanity check or raise an error if there are any processes which have not released this BS (unmapped)  */
	if(isbadbs(bs_id) || bsm_tab[bs_id].bs_total_proc){ 
		restore(ps);
		return SYSERR;
	}
	/* if BSM_UNMAPPED, do nothing and return OK  */
	if(bsm_tab[bs_id].bs_status == BSM_UNMAPPED)
		return OK;
	
	/* check for all the processes and raise an error if unmapped before release is called */
	for(i=0; i < NPROC; i++){
		if(bsm_tab[bs_id].bs_pid[i]){
			restore(ps);
			return SYSERR;
		}
	}	
	
	/* safe to release the BS */
	free_bsm(bs_id);	
	restore(ps);
   	return OK;
}

