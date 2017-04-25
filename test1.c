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
		signal(10, printHand);
		fib(40);
	} else {
		fib(40);
		sigsend(pid,10);
		wait();
	}

	exit();
	return 0;
} 
