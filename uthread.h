/* This header file defines all the user-space threading functions. */

#include <pthread.h>

struct pthreads
{
	pthread_t *ptid;
	struct pthreads *next;
};

struct tcb
{
	struct tcb *next;
	void *(*func) (void*);
	void *arg;
	int running;
	void *stack;
};

int uthread_init(void);

void uthread_create(void *(*func) (void*), void* arg);

void uthread_exit(void);

void uthread_yield(void);

void uthread_cleanup(void);

enum uthread_policy {
    UTHREAD_DIRECT_PTHREAD = 0,
    UTHREAD_PRIORITY,
    /* Add the next policy */
};

void uthread_set_policy(enum uthread_policy policy);

void uthread_set_param(int param);
