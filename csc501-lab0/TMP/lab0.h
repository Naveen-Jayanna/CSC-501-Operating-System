#ifndef _lab0_h_
#define _lab0_h_

#include <stdio.h>

extern int valid_process[];
extern unsigned long ctr1000;

struct sctrace {
	char *call_name;
	unsigned long call_freq;
	unsigned long call_time_taken;
}syscall_table[50][27];

extern long zfunction(long);        //Task 1
extern void printsegaddress();      //Task 2
extern void printprocstks(int);     //Task 3
extern void printtos();             //Task 4
extern void syscallsummary_start(); //Task 5
extern void syscallsummary_stop();  //Task 5
extern void printsyscallsummary();  //Task 5
#endif