#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <mark.h>
#include <bufpool.h>
#include <paging.h>

int write_bs(char *src, bsd_t bs_id, int page) {

	STATWORD ps;
	disable(ps);
	if(isbadbs(bs_id) || page < 0 || page >= NPBS){
		restore(ps);
		return SYSERR;
	}

	// kprintf("\n\n\nTesting ---------------------------------------- write_bs - write_bs() \n\n\n");
   char * phy_addr = BACKING_STORE_BASE + bs_id*BACKING_STORE_UNIT_SIZE + page*NBPG;
   bcopy((void*)src, phy_addr, NBPG);

	restore(ps);
	return OK;
}

