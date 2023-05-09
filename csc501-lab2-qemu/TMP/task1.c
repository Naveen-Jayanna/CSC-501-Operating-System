#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <lock.h>
#include <stdio.h>


#define DEFAULT_LOCK_PRIO 20

#define assert(x,error) if(!(x)){ \
            kprintf(error);\
            return;\
            }

/* Priority inversion problem in locks solved by priority inheritance */
void writer1(char *msg, int lck)
{
    kprintf ("  %s: to acquire lock\n", msg);
    lock (lck, WRITE, DEFAULT_LOCK_PRIO);
    kprintf ("  %s: acquired lock\n", msg);
    kprintf ("  %s: to release lock\n", msg);
    releaseall (1, lck);
}


void writer2(char *msg, int lck)
{
    kprintf ("  %s: to acquire lock\n", msg);
    lock (lck, WRITE, DEFAULT_LOCK_PRIO);
    kprintf ("  %s: acquired lock\n", msg);
    int i;
    // wasting CPU cycles
    for(i=0; i<100000; i++);
    for(i=0; i<100000; i++);
    kprintf ("  %s: to release lock\n", msg);
    releaseall (1, lck);
}

void writer3(char *msg, int lck)
{
    kprintf ("  %s: to acquire lock\n", msg);
    lock (lck, WRITE, DEFAULT_LOCK_PRIO);
    kprintf ("  %s: acquired lock, sleep 3s\n", msg);
    sleep(3);
    kprintf("  %s: writer is running again\n", msg);
    int i;
    // wasting CPU cycles
    for(i=0; i<100000; i++);
    for(i=0; i<100000; i++);
    kprintf ("  %s: to release lock\n", msg);
    releaseall (1, lck);
}

void process_cpu_bound (char *msg)
{
    int i;
    kprintf ("  %s: begins execution\n", msg);
    sleep(3);
    for(i=0; i<100000; i++);
    for(i=0; i<100000; i++); 
    kprintf ("  %s: ends execution\n", msg);
}

void testlock ()
{
    int     wr1,wr2, wr3;
    int 	proccpu1,proccpu2;
    int 	lck;

    kprintf("\n------------------------------------------Task 2-----------------------------------------------");
    kprintf("\n********** Priority inversion problem in locks solved by priority inheritance **********\n\n");
    lck  = lcreate ();
    assert (lck != SYSERR, "priority inversion problem in locks failed");
    wr1 = create(writer1, 2000, 90, "wr1", 2, "writer1", lck);
    wr2 = create(writer2, 2000, 50, "wr2", 2, "writer2", lck);
    wr3 = create(writer3, 2000, 20, "wr3", 2, "writer3", lck);
    proccpu1 = create(process_cpu_bound, 2000, 65, "proccpu1", 1, "proccpu1");
    proccpu2 = create(process_cpu_bound, 2000, 75, "proccpu2", 1, "proccpu2");

    kprintf("-start writer 3, then sleep 3s. lock granted to writer 3 (priority =  20)\n");
    resume(wr3);
    sleep (1);

    kprintf("-start writer 2, then sleep 3s. writer 2 (priority =  50) blocked on the lock\n");
    resume(wr2);

    kprintf("-start writer 1. writer 1 (priority =  90) blocked on the lock\n");
    resume(wr1);

    kprintf("-start proccpu1 (priority =  65)\n");
    resume (proccpu1);

    kprintf("-start proccpu2 (priority = 75)\n");
    resume (proccpu2);

    sleep (5);
}

/* Priority inversion problem in samaphores */

void semwriter1(char *msg, int sem)
{ 
    kprintf ("  %s: to acquire sem\n", msg);
    wait (sem);
    kprintf ("  %s: acquired sem\n", msg);
    kprintf ("  %s: to release sem\n", msg);
    signal (sem);
}

void semwriter2 (char *msg, int sem)
{
    int i;
    kprintf ("  %s: to acquire sem\n", msg);
    wait (sem);
    kprintf ("  %s: acquired sem, sleep 3s\n", msg);
    sleep(3);
    kprintf("  %s: writer is resumed \n", msg);
    // wasting CPU cycles
    for(i=0; i<100000; i++);
    for(i=0; i<100000; i++); 
    kprintf ("  %s: ends execution\n", msg);
    signal (sem);
}

void semwriter3 (char *msg, int sem)
{
    int i;
    kprintf ("  %s: to acquire sem\n", msg);
    wait (sem);
    kprintf ("  %s: acquired sem, sleep 3s\n", msg);
    sleep(3);
    kprintf("  %s: writer is resumed \n", msg);
    // wasting CPU cycles
    for(i=0; i<100000; i++);
    for(i=0; i<100000; i++); 
    kprintf ("  %s: ends execution\n", msg);
    signal (sem);
}

void testsem ()
{
    int     proccpu1, proccpu2;
    int     wr1, wr2, wr3;
    int     sem;

    printf("\n********** Semaphore - Problem of priority inversion **********\n\n");
    sem  = screate (1);
    assert (sem != SYSERR, "priority inversion problem in semaphore failed");

    wr1 = create(semwriter1, 2000, 90, "wr1", 2, "writer1", sem);
    wr2 = create(semwriter2, 2000, 50, "wr2", 2, "writer2", sem);
    wr3 = create(semwriter3, 2000, 20, "wr2", 2, "writer2", sem);
    proccpu1 = create(process_cpu_bound, 2000, 65, "proccpu1", 1, "proccpu1");
    proccpu2 = create(process_cpu_bound, 2000, 75, "proccpu2", 1, "proccpu2");

    kprintf("-start writer 3, then sleep 1s. semaphore granted to writer 3 (priority =  20)\n");
    resume(wr3);
    sleep (1);

    kprintf("-start writer 2. writer 2 (priority =  50) blocked on the semaphore\n");
    resume(wr2);

    kprintf("-start writer 1. writer 1 (priority =  90) blocked on the semaphore\n");
    resume(wr1);

    kprintf("-start proccpu1 (priority =  65)\n");
    resume (proccpu1);

    kprintf("-start proccpu2 (priority = 75)\n");
    resume (proccpu2);

    sleep (5);
}

// int main( )
// {
//     testlock();
//     testsem();
//     shutdown();
// }