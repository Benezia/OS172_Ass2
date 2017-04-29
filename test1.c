#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "uthread.h"

void fib(int n) {
  if (n <= 1)
    return;
  fib(n-1);
  fib(n-2);
}

void alarmLooper(int signum) {
	printf(1," RING \n");
	alarm(3);
}

void thread0Main(){
	int i;

	for (i=0;i <100; i++){
		printf(1," thread 0 (MT) is running now %d\n ",i);
	}
}

void thread1Main(){
	int i;	
	for (i=0;i <100; i++){
		printf(1,"thread 1 is running now %d\n",i);
	}
}
void thread2Main(){
	int i;
	
	for (i=0;i <100; i++){
		printf(1,"thread 2 is running now %d\n",i);
	}
}


void semaphoreTest() {
	int i;
	bsem_down(0);
	for (i = 0; i < 50; i++) {
		printf(1,"%d. tid: %d\n", i, uthread_self());
	}
	bsem_up(0);
}

void printHand(int sigNum){
	printf(1,"printHand handler was invoked \n");
}


int main(int argc, char *argv[]){
	uthread_init();
	printf(1, "allocated: %d\n", bsem_alloc()); //allocate #0 sem

	uthread_create(&semaphoreTest, 0);
	uthread_create(&semaphoreTest, 0);
	uthread_create(&semaphoreTest, 0);

	semaphoreTest();
	//uthread_create(&thread2Main, 0);
	//uthread_create(&thread3Main, 0);
	//signal(14,&alarmLooper);
	//sigsend(getpid(),14);
	//thread1Main();
	fib(35);
	printf(1,"Main Exit \n");
	exit();
	return 0;
} 
