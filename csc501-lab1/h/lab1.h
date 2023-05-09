#ifndef AGESCHED
#define AGESCHED 1
#endif

#ifndef LINUXSCHED
#define LINUXSCHED 2
#endif

//variable and functions related to Aging Based Scheduler
extern int schedclass;
void setschedclass(int);
int getschedclass();

//variables related to Linux-like Scheduler 
extern int current_priority;
extern int epoch;
