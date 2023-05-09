/* paging.h */

typedef unsigned int	 bsd_t;

/* Structure for a page directory entry */

typedef struct {

  unsigned int pd_pres	: 1;		/* page table present?		*/
  unsigned int pd_write : 1;		/* page is writable?		*/
  unsigned int pd_user	: 1;		/* is use level protection?	*/
  unsigned int pd_pwt	: 1;		/* write through cachine for pt?*/
  unsigned int pd_pcd	: 1;		/* cache disable for this pt?	*/
  unsigned int pd_acc	: 1;		/* page table was accessed?	*/
  unsigned int pd_mbz	: 1;		/* must be zero			*/
  unsigned int pd_fmb	: 1;		/* four MB pages?		*/
  unsigned int pd_global: 1;		/* global (ignored)		*/
  unsigned int pd_avail : 3;		/* for programmer's use		*/
  unsigned int pd_base	: 20;		/* location of page table?	*/
} pd_t;

/* Structure for a page table entry */

typedef struct {

  unsigned int pt_pres	: 1;		/* page is present?		*/
  unsigned int pt_write : 1;		/* page is writable?		*/
  unsigned int pt_user	: 1;		/* is use level protection?	*/
  unsigned int pt_pwt	: 1;		/* write through for this page? */
  unsigned int pt_pcd	: 1;		/* cache disable for this page? */
  unsigned int pt_acc	: 1;		/* page was accessed?		*/
  unsigned int pt_dirty : 1;		/* page was written?		*/
  unsigned int pt_mbz	: 1;		/* must be zero			*/
  unsigned int pt_global: 1;		/* should be zero in 586	*/
  unsigned int pt_avail : 3;		/* for programmer's use		*/
  unsigned int pt_base	: 20;		/* location of page?		*/
} pt_t;

typedef struct{
  unsigned int pg_offset : 12;		/* page offset			*/
  unsigned int pt_offset : 10;		/* page table offset		*/
  unsigned int pd_offset : 10;		/* page directory offset	*/
} virt_addr_t;

typedef struct{
  int bs_status;			/* MAPPED or UNMAPPED		*/
  int bs_pid[NPROC];				/* process id using this slot   */
  int bs_vpno[NPROC];					/* starting virtual page numbers for the processes which share the BS */
  int bs_npages;			/* number of pages in the store */
  int bs_sem;				/* semaphore mechanism ?	*/
  int bs_shared_priv;			/* BSM_SHARED OR BSM_PRIV	*/
  int bs_total_proc;			/* total processes with this BS  */
} bs_map_t;

typedef struct{
  int fr_status;			/* MAPPED or UNMAPPED		*/
  int fr_pid;				/* process id using this frame  */
  int fr_vpno;				/* corresponding virtual page no*/
  int fr_refcnt;			/* reference count		*/
  int fr_type;				/* FR_DIR, FR_TBL, FR_PAGE	*/
  int fr_dirty;
  int fr_sc;				/* SC page replacement */
  int fr_wtype;			/* write type for mapped page	*/
}fr_map_t;

/* FIFO queue Structure */
struct fifo_node{
  int key;
  struct fifo_node *next;
};

/*Inline manipulation procedures for FIFO queue */
#define fifo_isempty(list) (list == NULL)
#define fifo_firstkey(list) (list->key)

void append_fifo(struct fifo_node **, int);
int delete_fifo(struct fifo_node **, int);

extern int page_replace_policy;
extern int debug;

extern bs_map_t bsm_tab[];
extern fr_map_t frame_tab[];
extern int 	gpt_tab[];
extern struct fifo_node *fifohead;
extern int sc_index;
extern int sc_queue_pages;

/* Prototypes for required API calls */
SYSCALL xmmap(int, bsd_t, int);
SYSCALL xunmap(int);

/* given calls for dealing with backing store */

int get_bs(bsd_t, unsigned int);
SYSCALL release_bs(bsd_t);
SYSCALL read_bs(char *, bsd_t, int);
SYSCALL write_bs(char *, bsd_t, int);

/* paging helper functions */
int get_virtual_page_number(virt_addr_t, int *);
int get_virtual_address(unsigned long, virt_addr_t *);
int allocate_page_dir(int , int *);
void initialize_page_dir(int );
int allocate_page_table(int, int *);
void initialize_page_table(int);
void print_table();
void remove_page_table_entry(int);


#define NBPG		4096	/* number of bytes per page	*/
#define NPBS		128	/* number of pages per BS	*/
#define FVPP		4096	/* first mappable virtpage for process 	*/
#define FRAME0		1024	/* zero-th frame		*/
#define NFRAMES 	1024	/* number of frames		*/
// #define NFRAMES		10	/* page replacement	testing */
#define NBS		16	/* total backing stores	*/
#define NPTE	1024	/* total number of entries in a page table */
#define NPDE  1024    /* number of entries in a page directory */
#define NVP 1048575 /*lower limit = 4096, upper limit = 2^20 */

#define BSM_UNMAPPED	0
#define BSM_MAPPED	1

#define BSM_SHARED	0
#define BSM_PRIV	1

#define FRM_UNMAPPED	0
#define FRM_MAPPED	1

#define FR_PAGE		0
#define FR_TBL		1
#define FR_DIR		2

#define SC 3
#define FIFO 4

#define BACKING_STORE_BASE	0x00800000
#define BACKING_STORE_UNIT_SIZE 0x00080000

#define WR_BASIC 1
#define WR_FORCED 2

#define	isbadbs(x)	(x<0 || x>=NBS)
#define isbadvpno(x)	(x<4096 || x>=NVP)
