#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "uthread.h"

struct thread threadTable[MAX_UTHREADS];
int nexttid = 0;
struct thread* ct;
int threadSystemOn = 0;


void printError() {
	printf(2, "Thread System Uninitialized\n");
}

void initThreadTable(){
	int i;
	for (i=0;i<MAX_UTHREADS;i++) {
		threadTable[i].state = TERMINATED;
		threadTable[i].blockedOnSemaphore = -1;
		threadTable[i].joinOnTid = -1;
	}
}

int chooseNextThread(){
	int i = 0; 

	int currLoc;
	int startLoc = ct->indexInThreadtable;

	for (i = 1; i <= MAX_UTHREADS; i++) {
		currLoc = (startLoc + i)%MAX_UTHREADS;
		if (threadTable[currLoc].state == SLEEPING &&  threadTable[currLoc].wakeUpTime <= uptime())
			threadTable[currLoc].state = READY;
		if (threadTable[currLoc].state == READY){
			return currLoc;
		}
	}
	if (ct->state == RUNNING)
		return startLoc; //let current thread run again
	return -1; //no threads to run
}

int getThreadCount() {
	int threadCount = 0;
	int i;
	for (i = 0; i < MAX_UTHREADS; i++) {
		if (threadTable[i].state != TERMINATED) 
			threadCount++;
	}
	return threadCount;
}


int uthread_sleep(int ticks) {
	if (!threadSystemOn) {
		printError();
		return -1;
	}
	ct->wakeUpTime = ticks + uptime();
	ct->state = SLEEPING;
	sigsend(getpid(),SIGALRM);
	return 0;
}

void uthread_join(int tid){
	if (!threadSystemOn) {
		printError();
		return;
	}
	int i = 0;
	for (i = 0; i < MAX_UTHREADS; i++) {
		if (tid == threadTable[i].tid)
			break;
	}
	if (i == MAX_UTHREADS || threadTable[i].state == TERMINATED)
		return; //no match for tid or target state is terminated.
	ct->joinOnTid = threadTable[i].tid;
	ct->state = BLOCKED;
	sigsend(getpid(),SIGALRM);
}

void uthread_exit(){
	if (!threadSystemOn) {
		printError();
		return;
	}

	if (getThreadCount() == 1) { //is the last thread
		if (ct->tid != 0) 
			free(ct->stack);
		exit();
	}
	//if reached here, there are more threads in the process:

	int i;
	for (i = 0; i < MAX_UTHREADS; i++) {
		if (threadTable[i].joinOnTid == ct->tid) {
			threadTable[i].joinOnTid = -1;
			threadTable[i].state = READY; //release blocked threads (if any)
		}	
	}



	if (ct->tid != 0)
		free(ct->stack);
	ct->state = TERMINATED;
	sigsend(getpid(),SIGALRM);
}

void uthread_schedule(){
	int nextThread;
	int ebpSnapshot;

	register uint ebpVal asm("ebp");
	ebpSnapshot = ebpVal;

	ebpSnapshot += 12;				//+4 for sigret ptr +4 for signum +4 for function jump
 	ct->tf = *((struct threadtrapframe*)ebpSnapshot);	//backup current thread's trapframe
 	if (ct->state == RUNNING)
 		ct->state = READY; 			//prevent exited thread from running
	nextThread = chooseNextThread();
 	while (nextThread == -1) 		//busywaits until an available thread is found
 		nextThread = chooseNextThread();
 	
 	ct = &threadTable[nextThread];
 	*((struct threadtrapframe*)ebpSnapshot) = ct->tf;	//load next thread's trapframe
 	ct->state = RUNNING;

	alarm(UTHREAD_QUANTA);
}

void alarmHandler(int sigNum){
	 uthread_schedule();
}

int getNextSpot(){
	int i;
	for (i = 0; i<MAX_UTHREADS; i++){
		if (threadTable[i].state == TERMINATED)
			return i;
	}
	return -1;
}

int uthread_create(start_func threadEntry, void* arg){
	if (!threadSystemOn) {
		printError();
		return -1;
	}
	int ttableLoc;
	ttableLoc = getNextSpot();
	struct thread* t = &threadTable[ttableLoc] ;

	if (ttableLoc == -1)
		return -1;

	t->tid = nexttid;
	nexttid++;

	//STACK CREATION:
	t->joinOnTid = -1;
	t->stack = malloc(STACKSZ);
	t->tf = ct->tf;
	t->tf.esp = (uint)(t->stack + STACKSZ); 			//point to bottom of the stack
	t->tf.ebp = t->tf.esp;
	t->tf.eip = (uint)threadEntry;
	t->indexInThreadtable = ttableLoc;
	t->tf.esp -= 4; 									//make room for args;
	*((uint *)t->tf.esp) = (uint)arg;					//push args to stack
	t->tf.esp -= 4; 									//make room for the return address;
	*((uint *)t->tf.esp) = (uint)&uthread_exit;			//push return address to stack
	t->state = READY;
	return 0;
}

int uthread_init(){
	if (threadSystemOn)
		return -1;
	threadSystemOn = 1;
	initThreadTable(threadTable);

	threadTable[0].tid = nexttid;
	nexttid++;
	threadTable[0].indexInThreadtable = 0;
	threadTable[0].state = RUNNING;
	threadTable[0].joinOnTid = -1;
	ct = &threadTable[0];
	signal(SIGALRM,&alarmHandler);
	sigsend(getpid(),SIGALRM);

	return 0;
}



int uthread_self() {
	return ct->tid;
}


//===================== SEMAPHORES =====================
#define MAX_BSEM 128
static int semaphores[MAX_BSEM] = {[0 ... MAX_BSEM-1] = -1};


int bsem_alloc(){
	int i;
	for (i = 0; i < MAX_BSEM; i++) {
		if (semaphores[i] == -1) {
			semaphores[i] = 1;
			return i;
		} 
	}
	return -1;
}

void bsem_free(int semNum){
	int i;
	for (i = 0; i < MAX_UTHREADS; i++) {
		if (threadTable[i].blockedOnSemaphore == semNum) {
			threadTable[i].blockedOnSemaphore = -1;
			threadTable[i].state = READY;	//release all blocked threads
		}
	}
	semaphores[semNum] = -1;
}


void bsem_down(int semNum){
	if (semaphores[semNum] == -1)
		return; //uninitialized semaphore
        alarm(0);       //Disable uthread schedular.
	if (semaphores[semNum] == 0) {
		ct->blockedOnSemaphore = semNum;
		ct->state = BLOCKED;
		sigsend(getpid(),SIGALRM);
	}
	else {
		semaphores[semNum] = 0;
                alarm(UTHREAD_QUANTA);  //Enable uthread schedular
        }
}


void bsem_up(int semNum){
	if (semaphores[semNum] == -1)
		return; //uninitialized semaphore
        alarm(0);   //Disable uthread schedular
	int i = 0;
	for (i = 0; i < MAX_UTHREADS; i++) {
		if (threadTable[i].blockedOnSemaphore == semNum)  {
			threadTable[i].blockedOnSemaphore = -1;
			threadTable[i].state = READY; //release blocked thread
			break;
		}
	}
	if (i == MAX_UTHREADS) //no blocked threads were found
		semaphores[semNum] = 1;
        alarm(UTHREAD_QUANTA);  //Enable uthread schedular
}
