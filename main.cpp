/* This is the main body of the test program.
 * DO NOT write any of your core functionality here.
 * You are free to modify this file but be aware that
 * this file will be replace during grading. */

#include <stdio.h>
#include <unistd.h>
#include "uthread.h"

int uthread_policy = UTHREAD_DIRECT_PTHREAD;

void *thread1(void* arg)
{
    for (int i = 0; i < 10; i++) {
        printf("This is thread 1\n");
        usleep(10);
    }
    uthread_exit();
}

void *thread2(void* arg)
{
    for (int i = 0; i < 10; i++) {
        printf("This is thread 2\n");
        usleep(10);
    }
    uthread_exit();
}

void *thread3(void* arg)
{
    for (int i = 0; i < 10; i++) {
        printf("This is thread 3\n");
        usleep(10);
    }
    uthread_exit();
}

void *thread4(void* arg)
{
    for (int i = 0; i < 10; i++) {
        printf("This is thread 4\n");
        usleep(10);
    }
    uthread_exit();
}

void *thread5(void* arg)
{
    for (int i = 0; i < 10; i++) {
        printf("This is thread 5\n");
        usleep(10);
    }
    uthread_exit();
}

void *thread6(void* arg)
{
    for (int i = 0; i < 10; i++) {
        printf("This is thread 6\n");
        usleep(10);
    }
    uthread_exit();
}

int main(int argc, const char** argv)
{
    //uthread_set_policy(UTHREAD_DIRECT_PTHREAD);
    uthread_set_policy(UTHREAD_PRIORITY);
    if (uthread_init()) {
	printf("uthread_init failed\n");
	return 1;
    }

    uthread_create(thread1, NULL);
    uthread_create(thread2, NULL);
    uthread_create(thread3, NULL);
    uthread_create(thread4, NULL);
    uthread_create(thread5, NULL);
    uthread_create(thread6, NULL);
    uthread_cleanup();
//    usleep(100000);
    return 0;
}
