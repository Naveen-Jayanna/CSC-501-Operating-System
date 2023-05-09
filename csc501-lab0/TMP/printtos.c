/*printtos.c - printtos*/

#include <stdio.h>

unsigned long *ebp, *esp;
void printtos(){
    asm("movl %ebp, ebp");
	kprintf ("\nvoid printtos()\n");
    kprintf ("Before[0x%08x]: 0x%08x\n", ebp+2, *(ebp+2));
	kprintf ("After [0x%08x]: 0x%08x\n", ebp, *ebp);
    unsigned long temp1 = 0xfefe;
    unsigned long temp2 = 0xefef;
    // temp1 = temp1 + temp2;
    // asm("movl %esp, esp");
    kprintf ("\t element[0x%08x]: 0x%08x\n",ebp-1,*(ebp-1));
    kprintf ("\t element[0x%08x]: 0x%08x\n",ebp-2,*(ebp-2));
    kprintf ("\t element[0x%08x]: 0x%08x\n",ebp-3,*(ebp-3));
    kprintf ("\t element[0x%08x]: 0x%08x\n",ebp-4,*(ebp-4));

}