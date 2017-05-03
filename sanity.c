#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "uthread.h"
#include "usemaphore.h"

#define BUFSZ 20
#define ITMNO 200

int queue[BUFSZ];

int out;
int in;
struct counting_semaphore * empty;
struct counting_semaphore * full;
struct counting_semaphore * mutex;

int removeItem(){
	int retNum = queue[out];
	out = (out + 1) % BUFSZ;
	return retNum;
}

void addItem(int item){
	queue[in] = item;
	in = (in + 1) % BUFSZ;
}


void producer(void* arg){
	int i;
	printf(1,"producer-start\n");
	for (i = 1; i <= ITMNO; i++){
		down(empty);
		down(mutex);
		addItem(i);
		up(mutex);
		up(full);
	}
}
void consumer(void* arg){
	while (1){
		down(full);
		down(mutex);
		int item = removeItem();
		up(mutex);
		up(empty);
		uthread_sleep(item);
		printf(1, "Thread %d slept for %d ticks.\n", uthread_self(), item);
		if (item == ITMNO)
			exit();
	}
}




void test(void * arg){
	int i;
	for (i = 0; i < 200; i++)
		printf(1, "THREAD 1: %d\n", i);
}

void test0(void * arg){
	int i;
	for (i = 0; i < 200; i++)
		printf(1, "THREAD 0: %d\n", i);
}
void alarmer(int signum){
	printf(1, " DING ");
	alarm(3);
}



int main(int argc, char *argv[]){
	

 // 	uthread_init();
	// uthread_create(&test,0);
	// test0(0);
	// uthread_exit();

	uthread_init();
	empty = allocSem(BUFSZ);
	full = allocSem(0);
	mutex = allocSem(1);


	//printf(1,"empty: %d, full: %d, mutex %d\n",empty,full,mutex);
	uthread_create(&producer,0);
	uthread_create(&consumer,0);
	uthread_create(&consumer,0);
	uthread_create(&consumer,0);
	uthread_exit();
	// //exit();
	return 0;
} 
