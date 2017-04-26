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

void backUpTrapframe(struct threadtf *tf) {
	// ct->threadtf -> *tf;
{

// void backUpTrapframe(){

	// asm ("movl %%ebp, %0;"
	//      :"=r"(ct->ebp) 
	//      :
	//      :);

	// asm ("movl %%esp, %0;"
	//      :"=r"(ct->esp) 
	//      :
	//      :);

	// asm ("movl %%edi, %0;"
	//      :"=r"(ct->edi) 
	//      :
	//      :);

	// asm ("movl %%esi, %0;"
	//      :"=r"(ct->esi) 
	//      :
	//      :);

	// asm ("movl %%eax, %0;"
	//      :"=r"(ct->eax) 
	//      :
	//      :);



	// asm ("movl %%ebx, %0;"
	//      :"=r"(ct->ebx) 
	//      :
	//      :);

	// asm ("movl %%ecx, %0;"
	//      :"=r"(ct->ecx));

	// asm ("movl %%edx, %0;"
	//      :"=r"(ct->edx) 
	//      :
	//      :);

    // asm ("get_eip: mov (%%esp),%%eax;"
    //      "ret;"
    //      "call get_eip;"
    //      "mov %%eax,%0;"
	   //   :"=r"(ct->eip) 
	   //   :
	   //   :);
  
  // printf(1,"thread trapframe registers:%d %d %d %d %d %d %d %d\n", ct->eax,ct->ebx,ct->ecx,ct->edx,ct->esi,
  //                                                 ct->edi,ct->esp,ct->ebp);
 	// // int i =2;

 //  printf(1,"thread trapframe registers:%d \n", ct->eip);
       
 

//   getpid();

// }

void uthread_schedule(){
	//printf(1,"sched is on like donkey kong !\n");








	//backUpTrapframe();
	asmTrapframeBackup();
	//	printf(1,"sched is on like donkey kong111111 !\n");
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

	(t->threadtf).tid = nexttid++;
	// (t->threadtf).esp = (uint)tstack + STACKSZ - 1;
	// (t->threadtf).ebp = (uint)tstack + STACKSZ - 1;
	// (t->threadtf).eip = (uint)threadEntry;
	t->stack = tstack;

	// t->esp -= 4;
	// *((int *)t->esp) = (int)&uthread_exit;
	

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