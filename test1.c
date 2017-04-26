#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"


void fib(int n) {
  if (n <= 1)
    return;
  fib(n-1);
  fib(n-2);
}

void printHand(int sigNum){
	printf(1,"printHand handler was invoked \n");

}


int main(int argc, char *argv[]){

	int pid;
	if ((pid = fork()) == 0) {
		//signal(10, &printHand);
		printf(1,"child proc pid is: %d\n",getpid());
		alarm(20);
		sleep(50);	
		alarm(50);
		sleep(500);	
	} else {
		sleep(500);	
	//	sigsend(pid,14);
		wait();
	}

	exit();
	return 0;
} 
