#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "uthread.h"

struct thread* threadTable[MAX_UTHREADS];
int nexttid = 0;
struct thread* ct;

void initThreadTable(){
	int i;

	for (i=0;i<MAX_UTHREADS;i++)
		threadTable[i] = (struct thread*)0;
}

void alarmHandler(int sigNum){
	uthread_schedule();
}

void * get_eip () {
	//This function returns the return address of the get_eip
	return __builtin_return_address(0); //info: https://gcc.gnu.org/onlinedocs/gcc/Return-Address.html
	//idea taken from: 
	//http://stackoverflow.com/questions/18350021/how-to-print-exact-value-of-the-program-counter-in-c
}

//FOR DEBUGGING:
void printTrapframe() {
	  printf(1,"Thread TF Registers:\n");
	  printf(1,"\tebp: %x\n", ct->tf.ebp);
	  printf(1,"\teip: %x\n", ct->tf.eip);
	  printf(1,"\tesp: %x\n", ct->tf.esp);
	  printf(1,"\tedi: %x\n", ct->tf.edi);
	  printf(1,"\tesi: %x\n", ct->tf.esi);
	  printf(1,"\tebx: %x\n", ct->tf.ebx);
	  printf(1,"\tedx: %x\n", ct->tf.edx);
	  printf(1,"\tecx: %x\n", ct->tf.ecx);
	  printf(1,"\teax: %x\n", ct->tf.eax);
}

void backUpTrapframe(){

    register int ebpVal asm("ebp");
	ct->tf.ebp = ebpVal; //WORKS!

    register int espVal asm("esp");
	ct->tf.esp = espVal; //WORKS!

	register int ediVal asm("edi");
	ct->tf.edi = ediVal; //WORKS!

	register int esiVal asm("esi");
	ct->tf.esi = esiVal; //WORKS!

 	//register int eaxVal asm("eax");
	//ct->tf.eax = eaxVal; //DOESNT WORK, always zero, stops UM skedular from iterating.

 	//register int ebxVal asm("ebx");
	//ct->tf.ebx = ebxVal; //WORKS but causes trap13 somewhere.

 	//register int ecxVal asm("ecx");
	//ct->tf.ecx = ecxVal;  //DOESNT WORK, shows a completely different number from kernel and causes trap14.

 	//register int edxVal asm("edx");
	//ct->tf.edx = edxVal; //might be working (kernel edx is always zero), causes trap14.

    printf(1,"%p\n", get_eip()); //should be x
    printf(1,"%p\n", get_eip()); //should be x+4 (instead its x)
    printf(1,"%p\n", get_eip()); //should be x+8 (instead its x)
	ct->tf.eip = (uint)get_eip;  //very different result from kernel yet doesnt causes any traps.

	printTrapframe();
   	getpid();
}

void uthread_schedule(){
	//printf(1,"sched is on like donkey kong !\n");


	backUpTrapframe();
	//printf(1,"sched is on like donkey kong111111 !\n");
	alarm(UTHREAD_QUANTA);
}

int getNextSpot(){

	int i;

	for (i = 0; i<MAX_UTHREADS; i++){
		if (threadTable[i] == 0){
			return i;
		}
	}

	return -1;
}

int uthread_create(start_func threadEntry, void* arg){

	int ttableLoc;
	ttableLoc = getNextSpot();

	if (ttableLoc == -1)
		return -1;

	struct thread* t = (struct thread*)malloc(sizeof(struct thread));
	ct = t;
	unsigned char* tstack = (unsigned char*)malloc(STACKSZ);

	t->tid = nexttid++;
	(t->tf).esp = (uint)tstack + STACKSZ - 1;
	(t->tf).ebp = (uint)tstack + STACKSZ - 1;
	(t->tf).eip = (uint)threadEntry;
	t->stack = tstack;

	(t->tf).esp -= 4;
	 *((int *)(t->tf).esp) = (int)&uthread_exit;
	

	threadTable[ttableLoc] = t;


	return 0;
}

void uthread_exit(){

}


int uthread_init(){
	
	struct thread* mainThread = (struct thread*)malloc(sizeof(struct thread));
	mainThread->tid = 0;
	nexttid++;

	initThreadTable(threadTable);
	threadTable[0] = mainThread;
	signal(SIGALRM,&alarmHandler);
	alarm(UTHREAD_QUANTA);

	return 0;
}

// need to clear allocated memory
// todo- check when adding new thread is not ok
// check how to define thread trapframe