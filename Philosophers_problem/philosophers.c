// fixing philosophers dining problem
#include <stdio.h> 
#include <pthread.h> 
#include <semaphore.h> 
#include <unistd.h>

sem_t fork[5] = {1};
int i;

void philosopher(int i) {
    while (1) {
            think();
            sem_wait(fork[i]);
            sem_wait(fork[(i+1) % 5]);
            eat();
            sem_signal(fork[(i+1) % 5]);
            sem_signal(fork[i]);
        }
    }

// void main() {
//     parbegin(philosopher(0), philosopher(1),
//     philosopher(2), philosopher(3), philosopher(4));

//     return;
// }