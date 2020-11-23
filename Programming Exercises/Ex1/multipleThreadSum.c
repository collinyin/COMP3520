#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// global variables
struct arg_struct {
    int limit;
    int sum;
};

void * thread_sumer(void * arg) {

    struct arg_struct * arg_struct = (struct arg_struct *) arg; 

    for (int i = 0; i <= arg_struct->limit; i++) {
        arg_struct->sum += i;
    }

    pthread_exit(0);
}

int main(int argc, char * argv[]) {

    // to prevent seg fault
    if (argc < 2) {
        exit(-1);
    }

    int num_of_threads = argc - 1;
    pthread_t tids[num_of_threads];
    struct arg_struct thread_struct[num_of_threads];

    for (int i = 0; i < num_of_threads; i++) {
        
        pthread_attr_t attr;
        pthread_attr_init(&attr);

        thread_struct[i].limit = atoi(argv[i + 1]);
        thread_struct[i].sum = 0;
        pthread_create(&tids[i], &attr, thread_sumer, &thread_struct[i]);
    }

    for (int i = 0; i < num_of_threads; i++) {
        pthread_join(tids[i], NULL);
        printf("Sum for thread %d: %d\n", i, thread_struct[i].sum);
    }
    
    return 0;
}