#define MAX_UTHREADS 64
#define SIGALRM 14
#define UTHREAD_QUANTA 5
#define STACKSZ 4096

typedef void (*start_func)(void*);

enum thread_state {running, ready, blocked, terminated}; 

struct threadtf{
	uint ebp;   
	uint eip;
	uint esp;

	uint edi;
	uint esi;   
	uint ebx;
	uint edx;
	uint ecx;
	uint eax;
};

struct thread{
	int tid;
	void* stack;
	enum thread_state state;  
	struct threadtf tf;
};

void backUpTrapframe() ;
int uthread_create(start_func, void*);
void uthread_schedule(void);
void alarmHandler(int);
int uthread_init(void); 
void uthread_exit();