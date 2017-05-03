
struct counting_semaphore {
	int binSemNum1;
	int binSemNum2;
	int value;
};

struct counting_semaphore* allocSem(int);
void freeSem(struct counting_semaphore*);
void down(struct counting_semaphore*);
void up (struct counting_semaphore*);
