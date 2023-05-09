/*printsyscallsummary.c - printsyscallsummary*/

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include "lab0.h"

int syscall_start = 0;

void syscallsummary_start() {
	syscall_start = 1;
}

void syscallsummary_stop() {
	syscall_start = 0;
}

void printsyscallsummary() {
    
	kprintf ("\nvoid printsyscallsummary()\n");
	int process, calls, total_processes = NPROC;
	int entry_point;

	for(process = 0; process < total_processes; process++) {
		entry_point = 1;
		if(valid_process[process] == 1) {
			for(calls = 0; calls < 27; calls++) {
				if(syscall_table[process][calls].call_freq > 0) {
					if(entry_point == 1){
						kprintf("\nProcess [pid:%d]", process);
						entry_point = 0;
					}
					kprintf("\n\t\tSyscall: %s, count: %d, average execution time: %lu (ms)",
					syscall_table[process][calls].call_name, syscall_table[process][calls].call_freq, (syscall_table[process][calls].call_time_taken/syscall_table[process][calls].call_freq));

				}
			}
		}
	}
}
