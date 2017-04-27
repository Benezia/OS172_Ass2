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

void uthread_schedule(){
	printf(1,"sched is on like donkey kong- start !\n");

	int nextThread;
	register uint espVal asm("esp");
	espVal += 16;
 	ct->tf = *((struct threadtrapframe*)espVal);
 	ct->state = READY;

 	nextThread = chooseNextThread();
 	ct = &threadTable[nextThread];

 	*((struct threadtrapframe*)espVal) = ct->tf;
 	ct->state = RUNNING;

	printf(1,"sched is on like donkey kong- end !\n");
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
	printf(1,"next spot in thread table is: %d\n",ttableLoc);
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
