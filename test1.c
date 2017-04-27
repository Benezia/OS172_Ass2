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


void thread2Main(void* args){

	int i;
	
	for (i=0;i <100; i++){
		printf(1,"thread 2 is running now\n");
	}
}


void printHand(int sigNum){
	printf(1,"printHand handler was invoked \n");

}


int main(int argc, char *argv[]){
	
	//int i;
	uthread_init();

	uthread_create(&thread2Main, 0);

	fib(40);

	exit();
	return 0;
} 
