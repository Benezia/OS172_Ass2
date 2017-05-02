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

void thread0Main(){
	int i;

	for (i=0;i <200; i++){
		printf(1,"Thread0 is Running %d\n",i);
	}
}

void thread1Main(void* arg){
	int i;	
	for (i=0;i <200; i++){
		printf(1,"Thread1 is Running %d\n",i);
	}
}
void thread2Main(void* arg){
	int i;	
	for (i=0;i <200; i++){
		printf(1,"Thread2 is Running %d\n",i);
	}
}


void semaphoreTest(void* cSem) {
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
	uthread_init();
	uthread_create(thread1Main, 0);
	uthread_create(thread2Main, 0);
	thread0Main();
	uthread_join(2);
	printf(1,"Main Exit \n");
	exit();
	//uthread_exit();
	return 0;
} 
