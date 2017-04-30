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
	alarm(2);
}

void thread0Main(){
	int i;

	for (i=0;i <50; i++){
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
	int i;
	uthread_init();
	for (i=0;i <50; i++){
		printf(1,"T0 Run %d\n",i);
		if (i == 30) {
			uthread_create(&thread1Main,0);
			//uthread_create(&thread2Main,0);
			//uthread_sleep(50);
		}
	}	
	printf(1,"Main Exit \n");
	//exit();
	uthread_exit();
	return 0;
} 
