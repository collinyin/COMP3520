// implementing binary semaphore
#include <pthread.h>
#include<stdlib.h>
#include<stdio.h>

typedef struct {
    int value;
    pthread_mutex_t m_lock;
    pthread_cond_t c_wait;
} mylib_semaphore_t;

mylib_semaphore_t * mylib_semaphore_init(int value) {
    mylib_semaphore_t * sem = (mylib_semaphore_t *) malloc (sizeof(mylib_semaphore_t));
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    sem->m_lock = mutex;

    pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
    sem->c_wait = cond;
    sem->value = value;

    return sem;
}

void mylib_semaphore_wait(mylib_semaphore_t *sem) {
    sem->value--;

    if (sem->value < 0) {
        pthread_cond_wait(&sem->c_wait, &sem->m_lock);
    }

    return;
}

void mylib_semaphore_signal(mylib_semaphore_t *sem) {
    sem->value++;

    if (sem->value <= 0) {
        pthread_cond_signal(&sem->c_wait);
        pthread_mutex_unlock(&sem->m_lock);
    }

    return;
}

int main() {

    return 0;
}