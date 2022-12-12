/* Write your core functionality in this file. */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "uthread.h"

extern int uthread_policy;
struct pthreads *ptid_head = NULL;
pthread_mutex_t lock;
struct tcb *ready_queue = NULL;
struct tcb *current_thread = NULL;

#define NR_THREADS 4

void cleanup_ptid_list()
{
	struct pthreads *temp;
	struct tcb *tcb;

	printf("cleanup_ptid_list called\n");
	usleep(1000);
	while (ptid_head) {
		temp = ptid_head;
		pthread_join(*(ptid_head->ptid), NULL);
		ptid_head = ptid_head->next;
		free(temp);
	}

	printf("ptid_head cleanup done\n");
	usleep(1000);
	while (ready_queue) {
		tcb = ready_queue;
		ready_queue = ready_queue->next;
		free(tcb);
	}
}

void* handler(void* arg_2)
{
    struct tcb *tcb;
    int stack_length = 4096;
    void *stack;
    void *(*func) (void*);
    void *arg;

    while(1) {
        /* Select a user-space thread from the ready queue */
	pthread_mutex_lock(&lock);
	if (!ready_queue) {
		pthread_mutex_unlock(&lock);
		usleep(100);
		continue;
	}

        /* Take the thread off the ready queue */
	tcb = ready_queue;
	current_thread = tcb;
	ready_queue = ready_queue->next;
        /* Run the task of the user-space thread */
	if (!current_thread->running) {
		//printf("No running\n");
		//usleep(100);
		/* If this is the first time the uthread is running, switch to the user stack of the uthread */
		stack = current_thread->stack + 4096;
		asm volatile(
			"movq %0, %%rsp"
			:: "r" (stack)
		);

		//printf("No running 2\n");
		//usleep(100);
		asm volatile(
			"movq %0, %%rbp"
			:: "r" (stack)
		);

		//printf("No running 3\n");
		//usleep(100);
		current_thread->running = 1;
		//printf("No running 4\n");
		//usleep(100);
	}

	printf("Calling the func = %p\n", current_thread->func);
	func = current_thread->func;
	arg = current_thread->arg;
	tcb = current_thread;
	pthread_mutex_unlock(&lock);
	func(arg);

	free(tcb);
    }
    return NULL;
}

int uthread_init(void)
{
	int i;

	if (uthread_policy == UTHREAD_DIRECT_PTHREAD)
		return 0;

	if (pthread_mutex_init(&lock, NULL) != 0) {
		printf(" mutex init has failed\n");
		return 1;
	}

	for (i = 1; i <= NR_THREADS; i++) {
		struct pthreads *ptid_node;
		pthread_t thread_id;

		ptid_node = (struct pthreads *) malloc(sizeof(struct pthreads));
		if (!ptid_node) {
			printf ("failed to allocate the ptid node object\n");
			goto cleanup;
		}

		if (pthread_create(&thread_id, NULL, handler, NULL)) {
			printf ("user thread creation has failed\n");
			goto cleanup;
		}

		printf("pthread created\n");
		ptid_node->next = ptid_head;
		ptid_head = ptid_node;
		ptid_head->ptid = &thread_id;
	}

	printf("%d pthreads created successfuly\n", NR_THREADS);
	return 0;

cleanup:
	cleanup_ptid_list();
	return 1;
}

void uthread_create(void *(*func) (void*), void* arg)
{
	pthread_t thread_id;
	struct pthreads *ptid_node;
	struct tcb *tcb;

	if (uthread_policy != UTHREAD_DIRECT_PTHREAD)
		goto setup_uthread_priority;

	ptid_node = (struct pthreads *) malloc(sizeof(struct pthreads));
	if (!ptid_node) {
		printf ("failed to allocate the ptid node object\n");
		return;
	}

	if (pthread_create(&thread_id, NULL, func, arg)) {
		printf ("user thread creation has failed\n");
		return;
	}

	printf("pthread created(1)\n");
	ptid_node->next = ptid_head;
	ptid_head = ptid_node;
	ptid_head->ptid = &thread_id;
	return;

setup_uthread_priority:
	tcb = (struct tcb *) malloc(sizeof(struct tcb));
	if (!tcb) {
		printf ("failed to allocate tcb\n");
		return;
	}

	tcb->stack = malloc(4096);
	if (!tcb->stack) {
		printf("Failed to allocate the stack\n");
		free(tcb);
		return;
	}

	tcb->func = func;
	tcb->arg = arg;
	tcb->running = 0;

	pthread_mutex_lock(&lock);
	tcb->next = ready_queue;
	ready_queue = tcb;
	pthread_mutex_unlock(&lock);
	printf("added one uthread to ready queue, callback = %p\n", func);
}

void uthread_exit(void)
{
	if (uthread_policy == UTHREAD_DIRECT_PTHREAD)
		pthread_exit(NULL);
}

void uthread_yield(void)
{
	pthread_yield();
}

void uthread_cleanup(void)
{
	printf ("uthread_cleanup called\n");
	cleanup_ptid_list();
}

void uthread_set_policy(enum uthread_policy policy)
{
	uthread_policy = policy;
}

void uthread_set_param(int param)
{
	return;
}
