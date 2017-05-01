#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "uthread.h"
#include "usemaphore.h"


void fib(int n) {
  if (n <= 1)
    return;
  fib(n-1);
  fib(n-2);
}

void alarmLooper(int signum) {
	printf(1," RING \n");
	alarm(2);
}

void thread0Main(){
	int i;

	for (i=0;i <200; i++){
		printf(1,"T0 Run %d\n",i);
	}
}

void thread1Main(){
	int i;	
	for (i=0;i <50; i++){
		printf(1,"T1 Run %d\n",i);
	}
}
void thread2Main(){
	int i;
	
	for (i=0;i <50; i++){
		printf(1,"T2 Run %d\n",i);
	}
}


void semaphoreTest(struct counting_semaphore * cSem) {
	int i;
	down(cSem);
	for (i = 0; i < 50; i++) {
		printf(1,"%d. tid: %d\n", i, uthread_self());
	}
	up(cSem);
}

void printHand(int sigNum){
	printf(1,"printHand handler was invoked \n");
}


int main(int argc, char *argv[]){
	//signal(14,&alarmLooper);
	//sigsend(getpid(),14);
	//thread0Main();

 	struct counting_semaphore * cSem = allocSem(2);
	uthread_init();
	uthread_create((start_func)&semaphoreTest, &cSem);

 	semaphoreTest(cSem);


	printf(1,"Main Exit \n");
	exit();
	//uthread_exit();
	return 0;
} 
