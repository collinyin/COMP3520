#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_THREADS 5

void * printHello(void * threadId) {
    int * tId;
    tId = (int *) threadId;
    printf("Hello, this is thread: #%d\n", *tId);
    pthread_exit(NULL);
}

int main(int argc, char * argv[]) {
    
    pthread_t threads[NUM_THREADS];
    int threadError, t, tIds[NUM_THREADS];

    for (t = 0; t < NUM_THREADS; t++) {
        printf("In main, creating thread %d\n", t);
        tIds[t] = t;
        threadError = pthread_create(&threads[t], NULL, printHello, (void *) &tIds[t]);
        
        if (threadError) {
            printf("ERROR; return code from pthread_create #%d\n", t);
            exit(-1);
        }
    }

    return 0;
}