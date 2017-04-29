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

void thread1Main(){
	int i;
	printf(1," thread 1 (MT) is running now 0\n");
	uthread_sleep(40);

	for (i=0;i <100; i++){
		printf(1," thread 1 (MT) is running now %d\n ",i);
	}
}

void thread2Main(){
	int i;	
	for (i=0;i <100; i++){
		printf(1,"thread 2 is running now %d\n",i);
	}
}
void thread3Main(){
	int i;
	
	for (i=0;i <100; i++){
		printf(1,"thread 3 is running now %d\n",i);
	}
}

void printHand(int sigNum){
	printf(1,"printHand handler was invoked \n");
}


int main(int argc, char *argv[]){
	uthread_init();

	uthread_create(&thread2Main, 0);
	//uthread_create(&thread3Main, 0);
	//signal(14,&alarmLooper);
	//sigsend(getpid(),14);
	uthread_join(1);
	thread1Main();
	//fib(40);
	printf(1,"Main Exit \n");
	exit();
	return 0;
} 
