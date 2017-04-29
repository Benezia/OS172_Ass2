#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "uthread.h"

struct thread threadTable[MAX_UTHREADS];
int nexttid = 0;
struct thread* ct;
int threadSystemOn = 0;

void initThreadTable(){
	int i;
	for (i=0;i<MAX_UTHREADS;i++) {
		threadTable[i].state = TERMINATED;
		threadTable[i].blockedOnSemaphore = -1;
	}
}


int chooseNextThread(){
	int i = 0;
	int currloc = ct->indexInThreadtable + 1;
	while (i<MAX_UTHREADS){
		currloc = currloc%MAX_UTHREADS;
		if (threadTable[currloc].state == READY || (threadTable[currloc].state == BLOCKED
												&&  threadTable[currloc].wakeUpTime <= uptime())){
			threadTable[currloc].state = READY;
			return currloc;
		}
		currloc++;
		i++;
	}
	return ct->indexInThreadtable;
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
	ct->state = BLOCKED;
	sigsend(getpid(),SIGALRM);
	while(ct->state != RUNNING);
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
	while (threadTable[i].state != TERMINATED) {
		uthread_sleep(1);
	}
}


void uthread_exit(){
	if (!threadSystemOn) {
		printError();
		return;
	}
	ct->state = TERMINATED;
	printf(1,"terminated tid: %d\n", ct->tid);
	if (getThreadCount() == 0) { //is the last thread
		free(ct->stack);
		exit();
	}
	//if reached here, there are more threads in the process:
	free(ct->stack);
	sigsend(getpid(),SIGALRM);
	while(1); //busywaits until thead schedular starts running
}

//FOR DEBUGGING:
void printTrapframe() {
	printf(1,"\tss: %x\n", ct->tf.ss);
    printf(1,"\tcs: %x\n", ct->tf.cs);
    printf(1,"\tds: %x\n", ct->tf.ds);
    printf(1,"\tes: %x\n", ct->tf.es);
    printf(1,"\tfs: %x\n", ct->tf.fs);
    printf(1,"\tgs: %x\n", ct->tf.gs);
    printf(1,"\ttrapno: %x\n", ct->tf.trapno);
    printf(1,"\terr: %x\n", ct->tf.err);
	printf(1,"\tebx: %x\n", ct->tf.ebx);
	printf(1,"\tedx: %x\n", ct->tf.edx);
	printf(1,"\tecx: %x\n", ct->tf.ecx);
	printf(1,"\teax: %x\n", ct->tf.eax);
	printf(1,"\teip: %x\n", ct->tf.eip);
	printf(1,"\tesp: %x\n", ct->tf.esp);
	printf(1,"\tebp: %x\n\n", ct->tf.ebp);
}


void uthread_schedule(){

	int nextThread;
	int espSnapShot;
	register uint espVal asm("esp");
	espSnapShot = espVal;	//because espVal is the actual register (shouldn't be changed)
	//espSnapShot += 20;
	espSnapShot += 36;		//8 for unknown reasons +4 for sigret ptr +4 for signum +4 for function jump
							//Add 16 if uthread_schedule contains printf
 	ct->tf = *((struct threadtrapframe*)espSnapShot);	//backup current thread's trapframe
	//printf(1,"OLD Thread TF Registers:\n");
 	//printTrapframe();
 	if (ct->state == RUNNING)
 		ct->state = READY; //prevent exited thread from running

 	nextThread = chooseNextThread();
 	ct = &threadTable[nextThread];
	//printf(1,"New thread was chosen: %d\n", nextThread);
	//printf(1,"NEW Thread TF Registers:\n");
 	//printTrapframe();
 	*((struct threadtrapframe*)espSnapShot) = ct->tf;	//load next thread's trapframe
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
	t->wakeUpTime = -1;
	nexttid++;

	//STACK CREATION:
	t->stack = malloc(STACKSZ);
	t->tf = ct->tf;
	t->tf.esp = (uint)(t->stack + STACKSZ); 			//point to bottom of the stack
	t->tf.ebp = t->tf.esp;
	t->tf.eip = (uint)threadEntry;
	t->indexInThreadtable = ttableLoc;
	t->tf.esp -= 4; 									//make room for the return address;
	*((uint *)t->tf.esp) = (uint)&uthread_exit;			//push return address to stack
	t->arg = arg;
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
	ct = &threadTable[0];
	signal(SIGALRM,&alarmHandler);
	sigsend(getpid(),SIGALRM);

	return 0;
}

void printError() {
	printf(2, "Thread System Uninitialized");
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
	semaphores[semNum] = -1;
}


void bsem_down(int semNum){
	if (semaphores[semNum] == -1)
		return; //uninitialized semaphore
	if (semaphores[semNum] == 0) {
		ct->blockedOnSemaphore = semNum;
		ct->state = LOCKED;
		sigsend(getpid(),SIGALRM);
		while(ct->state != RUNNING); //USING BUSYWAIT! 
	}
	else
		semaphores[semNum]--;
}


void bsem_up(int semNum){
	if (semaphores[semNum] == -1)
		return; //uninitialized semaphore
	int i = 0;
	for (i = 0; i < MAX_UTHREADS; i++) {
		if (threadTable[i].blockedOnSemaphore == semNum) 
			break;
	}

	if (i != MAX_UTHREADS) {
		threadTable[i].blockedOnSemaphore = -1;
		threadTable[i].state = READY;
	}
	else 
		semaphores[semNum]++;
}


