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

void printHand(int sigNum){
	printf(1,"printHand handler was invoked \n");

}


int main(int argc, char *argv[]){
	int i;
	uthread_init();

	for (i=0;i<1000;i++){
		sleep(1);
	}
	exit();
	return 0;
} 
