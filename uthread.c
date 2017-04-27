#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "uthread.h"

struct thread threadTable[MAX_UTHREADS];
int nexttid = 0;
struct thread* ct;


void initThreadTable(){
	int i;

	for (i=0;i<MAX_UTHREADS;i++)
		threadTable[i].state = TERMINATED;
}


int chooseNextThread(){

	int i = 0;
	int currloc = ct->indexInThreadtable + 1;

	while (i<MAX_UTHREADS){
		currloc = currloc%32;
		if (threadTable[currloc].state == READY){
			return currloc;
		}
		i++;
	}
	return ct->indexInThreadtable;
}


void uthread_exit(){

}

//FOR DEBUGGING:
void printTrapframe() {
	  printf(1,"\tebx: %x\n", ct->tf.ebx);
	  printf(1,"\tedx: %x\n", ct->tf.edx);
	  printf(1,"\tecx: %x\n", ct->tf.ecx);
	  printf(1,"\teax: %x\n", ct->tf.eax);
	  printf(1,"\teip: %x\n\n", ct->tf.eip);
}


void uthread_schedule(){
	//printf(1,"sched is on like donkey kong- start !\n");

	int nextThread;
	int espSnapShot;
	register uint espVal asm("esp");
	espSnapShot = espVal;	//because espVal is the actual register (shouldn't be changed)
	espSnapShot += 36;		//24 for unknown reasons +4 for sigret ptr +4 for signum +4 for handler overloading
 	ct->tf = *((struct threadtrapframe*)espSnapShot);	//backup current thread trapframe
	printf(1,"OLD Thread TF Registers:\n");
 	printTrapframe();
 	ct->state = READY;

 	nextThread = chooseNextThread();
 	ct = &threadTable[nextThread];
	printf(1,"NEW Thread TF Registers:\n");
 	printTrapframe();
 	*((struct threadtrapframe*)espSnapShot) = ct->tf;	//load next thread trapframe
 	ct->state = RUNNING;

	//printf(1,"sched is on like donkey kong- end !\n");
	alarm(UTHREAD_QUANTA);
}

void alarmHandler(int sigNum){
	 uthread_schedule();
}

int getNextSpot(){
	int i;
	for (i = 0; i<MAX_UTHREADS; i++){
		if (threadTable[i].state == TERMINATED){
			return i;
		}
	}
	return -1;
}

int uthread_create(start_func threadEntry, void* arg){

	int ttableLoc;
	ttableLoc = getNextSpot();
	//printf(1,"next spot in thread table is: %d\n",ttableLoc);
	printf(1,"entry location (eip): %p\n",threadEntry);
	struct thread* t = &threadTable[ttableLoc] ;

	if (ttableLoc == -1)
		return -1;

	t->tid = nexttid;
	nexttid++;
	t->tf.esp = (uint)(t->stack + STACKSZ - 1);
	t->tf.ebp = t->tf.esp;
	t->tf.eip = (uint)threadEntry;
	t->indexInThreadtable = ttableLoc;
	t->tf.esp -= 4;
	 *((uint *)t->tf.esp) = (uint)&uthread_exit;
	t->arg = arg;
	t->state = READY;

	return 0;
}

int uthread_init(){

	initThreadTable(threadTable);

	threadTable[0].tid = nexttid;
	nexttid++;
	threadTable[0].indexInThreadtable = 0;
	threadTable[0].state = RUNNING;
	ct = &threadTable[0];
	signal(SIGALRM,&alarmHandler);
	alarm(UTHREAD_QUANTA);

	return 0;
}
