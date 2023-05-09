#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

int get_bs(bsd_t bs_id, unsigned int npages) {
	if(isbadbs(bs_id) || npages == 0 || npages > NPBS ||
          (bsm_tab[bs_id].bs_shared_priv == BSM_PRIV) )
		return SYSERR;

  // kprintf("\n\n\nTesting ---------------------------------------- get_bs - get_bs()  bs_id = %d, bsm_tab[bs_id].bs_status = %d\n\n\n", bs_id, bsm_tab[bs_id].bs_status);

	if( (bsm_tab[bs_id].bs_status == BSM_MAPPED) && (bsm_tab[bs_id].bs_npages != -1) )
		return bsm_tab[bs_id].bs_npages;

	bsm_tab[bs_id].bs_status = BSM_MAPPED;
    return npages;
}


