/* frame.c - manage physical frames */
#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

LOCAL int get_page_to_replace();
LOCAL void update_sc_index();
extern int debug;
/*-------------------------------------------------------------------------
 * init_frm - initialize frame_tab
 *-------------------------------------------------------------------------
 */
SYSCALL init_frm()
{
	STATWORD ps;
	disable(ps);
	int i;
	for(i=0; i < NFRAMES; i++){ 
		frame_tab[i].fr_status = FRM_UNMAPPED;
		frame_tab[i].fr_pid = -1;
		frame_tab[i].fr_vpno = -1;
		frame_tab[i].fr_refcnt = 0;
		frame_tab[i].fr_type = -1;
		frame_tab[i].fr_dirty = 0;
        frame_tab[i].fr_sc = 0;
		frame_tab[i].fr_wtype = -1; 
	}

	/* for page replacement */
	fifohead = NULL;
	sc_index = -1;
	sc_queue_pages = 0;

	restore(ps);
  	return OK;
}

/*-------------------------------------------------------------------------
 * get_frm - get a free frame according page replacement policy
 *-------------------------------------------------------------------------
 */
SYSCALL get_frm(int* avail)
{
	STATWORD ps;
	disable(ps);

	int i, replpg, vpage, pid;

	for(i=0; i < NFRAMES; i++){
		if(frame_tab[i].fr_status == FRM_UNMAPPED){
			*avail = i;
			restore(ps);
			return OK;
		}
	}

	/* get the page replace policy */
	replpg = get_page_to_replace();

	// kprintf("\n\n\nTesting ---------------------------------------- frame.c - get_frm() replpg = %d\n\n\n", replpg);
	if(debug == 1){
		kprintf("The page being replaced = %d\n",FRAME0+replpg);
	}
	/* free the page */
	vpage = frame_tab[replpg].fr_vpno;
	pid = frame_tab[replpg].fr_pid;
        
	free_frm(replpg);
	*avail = replpg;		

	restore(ps);
  	return OK;
}

/*-------------------------------------------------------------------------
 * free_frm - free a frame 
 *-------------------------------------------------------------------------
 */
SYSCALL free_frm(int i)
{
	STATWORD ps;
	disable(ps);

	if(frame_tab[i].fr_type == FR_PAGE)
	{
		/* removing the page table entry w.r.t. to the frame */
		remove_page_table_entry(i);

		/* deleting the frame from the FIFO */
		if(delete_fifo(&fifohead, i) == SYSERR){
			restore(ps);
			return SYSERR;
		}
		/* As SC follows circular queue, last frame is removed */
		sc_queue_pages--;
		if(sc_queue_pages==0)
			sc_index = -1;
	}

	// Setting the frame table values
	frame_tab[i].fr_status = FRM_UNMAPPED;
	frame_tab[i].fr_pid = -1;
	frame_tab[i].fr_vpno = -1;
	frame_tab[i].fr_type = -1;
	frame_tab[i].fr_wtype = -1;
	frame_tab[i].fr_refcnt = 0;
	frame_tab[i].fr_dirty = 0;
	frame_tab[i].fr_sc = 0;				

	restore(ps);
  	return OK;
}


/*-------------------------------------------------------------------------
 * get_page_to_replace - select page replacement policy
 *-------------------------------------------------------------------------
 */
LOCAL int get_page_to_replace(){
	int i;
	if(page_replace_policy == SC){				// Selecting SC page replacement policy
		// PASS 1
		i = sc_index;
		do{
			if(frame_tab[i].fr_status == FRM_MAPPED && frame_tab[i].fr_type == FR_PAGE){
				if(!frame_tab[i].fr_sc){		// if the value is 0 (given a second chance) => the frame is replaced
					update_sc_index();
					return i;
				}
				frame_tab[i].fr_sc = 0;			// Setting the value to 0 => the frame gets a second chance
			}	
			i++;
			if(i >= NFRAMES)
				i = 0;
		}while(i != sc_index);

		// PASS 2
		i = sc_index;
		update_sc_index();
		return i;
	}	

	else if(page_replace_policy == FIFO)		// Selecting FIFO page replacement policy
		return fifo_firstkey(fifohead);	

	return SYSERR;
}


/*--------------------------------------------------------------------------------------------------------------
* update_sc_index - if there are more than 1 page, the sc_index points to the next value in the circular queue
*---------------------------------------------------------------------------------------------------------------
*/
LOCAL void update_sc_index(){
	int i;
	i = sc_index + 1;
	if(i >= NFRAMES)
		i = 0;
	while(i != sc_index){
		if(frame_tab[i].fr_status == FRM_MAPPED && frame_tab[i].fr_type == FR_PAGE){
			sc_index = i;
			break;
		}
		i++;
		/* wrap around */
		if(i >= NFRAMES)
			i = 0;
	}
}


