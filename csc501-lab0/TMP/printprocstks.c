/*printprocstks.c - printprocstks*/

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>

unsigned long *esp;

void printprocstks(int priority){
    kprintf ("\nvoid printprocstks(int priority)\n");
    int total_processes=NPROC, i;
    struct pentry *current_process;
    for (i=0; i<total_processes; i++){
        current_process = &proctab[i];
        if((current_process->pstate != PRFREE) && (current_process->pprio > priority)){
            kprintf ("Process [%s]\n", current_process->pname);
            kprintf ("\tpid: %d\n", i);
            kprintf ("\tpriority: %d\n", current_process->pprio);
            kprintf ("\tbase: 0x%08x\n", current_process->pbase);
            kprintf ("\tlimit: 0x%08x\n", current_process->plimit);
            kprintf ("\tlen: %d\n", current_process->pstklen);
            if(current_process->pstate == PRCURR){
                asm("movl %esp, esp");
            }
            else{
                esp = (unsigned long *)current_process->pesp;
            }
            kprintf("\tpointer: 0x%08x\n",esp);
        }
    }
}