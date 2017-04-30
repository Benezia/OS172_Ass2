#define MAX_UTHREADS 6
#define SIGALRM 14
#define UTHREAD_QUANTA 5
#define STACKSZ 4096

typedef void (*start_func)(void*);

enum thread_state {RUNNING, READY, BLOCKED, LOCKED, TERMINATED}; 


struct threadtrapframe {
  uint edi;
  uint esi;
  uint ebp;
  uint oesp;
  uint ebx;
  uint edx;
  uint ecx;
  uint eax;

  ushort gs;
  ushort padding1;
  ushort fs;
  ushort padding2;
  ushort es;
  ushort padding3;
  ushort ds;
  ushort padding4;
  uint trapno;

  uint err;
  uint eip;
  ushort cs;
  ushort padding5;
  uint eflags;

  uint esp;
  ushort ss;
  ushort padding6;
};

struct thread{
	int tid;
	unsigned char* stack;
	enum thread_state state;  
	struct threadtrapframe tf;
	int indexInThreadtable;
	void* arg;
	int wakeUpTime;
	int blockedOnSemaphore;
};

int uthread_sleep(int);
void uthread_join(int);
int uthread_self();
int chooseNextThread();
int uthread_create(start_func, void*);
void uthread_schedule(void);
void alarmHandler(int);
int uthread_init(void); 
void uthread_exit();
void printError();

//SEMAPHORES:
int bsem_alloc();
void bsem_free(int);
void bsem_down(int);
void bsem_up(int);

