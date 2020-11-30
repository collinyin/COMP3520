// implementing monitor (question 5. of final exam)
#include <pthread.h>
#include<stdlib.h>
#include<stdio.h>

// imagine this is a monitor
    // global variable
    int N;
    int curr_sum = 0;
    pthread_cond_t c_wait = PTHREAD_COND_INITIALIZER;
    pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

    // procedures
    void access_file(int num) {
        while (num + curr_sum >= N) {
            pthread_cond_wait(&c_wait, &lock);
        }
        curr_sum += num;
        
        return;
    }

    void finished_access(int num) {
        curr_sum -= num;
        pthread_cond_signal(&c_wait);
        pthread_mutex_unlock(&lock);

        return;
    }


int main() {

    return 0;
}