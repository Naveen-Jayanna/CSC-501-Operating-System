/* policy.c = srpolicy*/

#include <conf.h>
#include <kernel.h>
#include <paging.h>

extern int debug;
extern int page_replace_policy;
/*-------------------------------------------------------------------------
 * srpolicy - set page replace policy 
 *-------------------------------------------------------------------------
 */
SYSCALL srpolicy(int policy)
{
	STATWORD ps;
	disable(ps);

	/* sanity check */
	if(policy != SC && policy != FIFO){
		restore(ps);
		return SYSERR;			
	}
	debug = 1;
	page_replace_policy = policy;
	restore(ps);
  	return OK;
}

/*-------------------------------------------------------------------------
 * grpolicy - get page replace policy 
 *-------------------------------------------------------------------------
 */
SYSCALL grpolicy()
{
  return page_replace_policy;
}
