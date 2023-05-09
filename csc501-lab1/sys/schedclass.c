#include "lab1.h"

int schedclass;

int getschedclass(){
    return schedclass;
}

void setschedclass(int set_scheduler){
    if(set_scheduler == AGESCHED || set_scheduler == LINUXSCHED)
        schedclass = set_scheduler;
}
