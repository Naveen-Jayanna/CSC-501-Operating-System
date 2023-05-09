/* pfintr.S - pfintr */


    	   .text
pferrcode: .long 0
           .globl  pfintr,pferrcode 
pfintr:

	pop pferrcode		/*Pop the errorcode from the top of the stack and store it in the global variable pferrcode. */
	cli			        /* Disable all maskable interrupts by clearing the interrupt flag in the EFLAGS register. */
	pushfl			    /* Save the current EFLAGS register on the stack. */
	pushal			    /* Push all general-purpose registers onto the stack.*/
	call pfint		    /* Call the Page Fault Interrupt Service Routine (ISR). */
	popal			    /* Pop all general-purpose registers from the stack. */
	popfl			    /* Pop the saved EFLAGS register from the stack. */
	sti			        /* Re-enable all maskable interrupts by setting the interrupt flag in the EFLAGS register. */
	iret			    /* Return from the interrupt handler using the iret instruction.*/

