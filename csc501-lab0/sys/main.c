/* user.c - main */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <lab0.h>

int prX;
void halt();

/*------------------------------------------------------------------------
 *  main  --  user main program
 *------------------------------------------------------------------------
 */
prch(c)
char c;
{
	int i;
	sleep(5);	
}
int main()
{
	unsigned int x = 0x76543210;
	char *c = (char*) &x;
	if (*c == 0x10)
	{
		printf ("Underlying architecture is little endian. \n");
	}
	else
	{
		printf ("Underlying architecture is big endian. \n");
	}
	kprintf("---------------------------------Task 1---------------------------------\n");
	long param = 0xaabbccdd;
	long result = zfunction(param);
	kprintf("Value after bit manipulation:0x%08x\n", result);
	kprintf("---------------------------------Task 2---------------------------------\n");
	printsegaddress();
	kprintf("---------------------------------Task 3---------------------------------\n");
	printtos();
	kprintf("---------------------------------Task 4---------------------------------\n");
	printprocstks(1);
	kprintf("---------------------------------Task 5---------------------------------\n");
	syscallsummary_start();
	resume(prX = create(prch,2000,20,"proc X",1,'A'));
	sleep(10);
	syscallsummary_stop();
	printsyscallsummary();
	return 0;
}
