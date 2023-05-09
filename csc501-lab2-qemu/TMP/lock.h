#ifndef _LOCK_H_
#define _LOCK_H_

#define READ    10
#define WRITE   11
#ifndef	NLOCKS
#define	NLOCKS  50	/*  Total number of locks   */
#endif

#define	LFREE	'\01'		/*  State of the lock is FREE   */
#define	LUSED	'\02'		/*  State of the lock is USED   */

/* lock table entry */

struct	lentry	{
	char lstate;		        /* stores the current state = LFREE or LUSED			*/
	int ltype;		            /* lock acquired by type - READ or WRITE			*/
	int lprio;		            /* highest priority amongst all the processes waiting for this lock 	*/
	int	lock_acquired[50];	    /* list of all the processes which has acquired this lock	*/
	int	lock_deleted;	        /* once the value is deleted - value 1, else 0  */
	int	l_active_readers;	    /* Total number of active readers havin this lock*/
	int	lcnt;	                /* total count of this lock	*/
	int	lqhead;		            /* head of list  */
	int	lqtail;		            /* tail of list	*/
};
extern	struct	lentry	locks[];
extern	int	nextlock;

#define	isbadlock(curr_lock)	(curr_lock<0 || curr_lock>=NLOCKS)

void linit (void);
void priority_inherit(struct pentry *);
int waiting_process_with_highest_priority(struct lentry *);

#endif