#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// global variable
int sum = 0;

void * thread_sumer(void * arg) {
    int * thread_limit = (int *) arg;

    for (int i = 1; i <= *thread_limit; i++) {
        sum += i;
    }

    pthread_exit(0);
}

int main(int argc, char * argv[]) {

    // If no arguments are entered => quit program
    if (argc <= 1) {
        exit(0);
    }

    // pthread_create takes in 4 arguments:
    // 1. pthread_t *, which is normally a pointer to threadId
    // 2. pthread_attr_t *, which is the thread attribute
    // 3. void *(*start_routine) (void *), which is a function pointer which the thread runs
    // 4. void * arg, which is an argument passed into the create function

    // 1. thread Id
    pthread_t tid;

    // 2. thread attribute
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    // 3. function pointer (above)

    // 4. argument
    int limit = atoi(argv[1]);

    pthread_create(&tid, &attr, thread_sumer, &limit);

    pthread_join(tid, NULL);

    printf("Sum is: %d\n", sum);

    return 0;
}