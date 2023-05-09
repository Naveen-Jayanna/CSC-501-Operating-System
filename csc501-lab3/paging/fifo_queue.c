/* fifo_queue.c */
#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>
#include <mem.h>


/*-------------------------------------------------------------------------
 * append_fifo - append an item into the FIFO queue
 *-------------------------------------------------------------------------
 */
void append_fifo(struct fifo_node ** phead, int item){
	struct fifo_node *new_node,*temp;
	new_node = (struct fifo_node *)getmem(sizeof(struct fifo_node));
	new_node->key = item;
	new_node->next = NULL;
    
	if(fifo_isempty(*phead)){
		*phead = new_node;
    }
	// traverse to the last node and append the new node
	else{
		temp = *phead;
		while(temp->next != NULL){
			temp = temp->next;
		}
		temp->next = new_node;
	}
}

/*-------------------------------------------------------------------------
 * delete_fifo - delete an item from the FIFO queue
 *-------------------------------------------------------------------------
 */
int delete_fifo(struct fifo_node **phead, int item){

	if(fifo_isempty(*phead))
		return SYSERR;
        
	struct fifo_node *temp, *prev;
	temp = *phead;
	prev = *phead;

	/* if the head matches, removing first node */
	if((*phead)->key == item){
		*phead = (*phead)->next;
		freemem((struct mblock *)temp, sizeof(struct fifo_node));
		return item;
	}

	/* else traverse to find a match*/
	while(temp != NULL && temp->key != item){
		prev = temp;
		temp = temp->next;
	}

	/* remove the node*/
	if(temp->key == item){
		prev->next = temp->next;
		freemem((struct mblock *)temp, sizeof(struct fifo_node));
        return item;
	}
    
	return SYSERR;
}