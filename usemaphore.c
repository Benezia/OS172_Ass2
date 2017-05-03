#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "uthread.h"
#include "usemaphore.h"

 struct counting_semaphore * allocSem(int enterVal) {
 	struct counting_semaphore *semPtr = (struct counting_semaphore *)malloc(sizeof(struct counting_semaphore));
	int binSemNum1 = bsem_alloc();
	int binSemNum2 = bsem_alloc();
	if (binSemNum1 == -1 || binSemNum2 == -1)
		return 0; //There are no binary semaphores available to implement a counting semaphore
	semPtr->binSemNum1 = binSemNum1;
	semPtr->binSemNum2 = binSemNum2;
	semPtr->value = enterVal;
 	return semPtr;
 }

void freeSem(struct counting_semaphore *sem) {
	bsem_free(sem->binSemNum1);
	bsem_free(sem->binSemNum2);
	free(sem);
}

void down(struct counting_semaphore *sem) {
	bsem_down(sem->binSemNum2);
	bsem_down(sem->binSemNum1);
	sem->value--;
	if (sem->value > 0)
		bsem_up(sem->binSemNum2);
	bsem_up(sem->binSemNum1);
}
void up (struct counting_semaphore *sem) {
	bsem_down(sem->binSemNum1);
	sem->value++;
	if (sem->value == 1)
		bsem_up(sem->binSemNum2);
	bsem_up(sem->binSemNum1);
}
