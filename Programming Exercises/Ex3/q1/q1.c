#include "q1.h"
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

int main() {

    /* Question 1.
    
    - create process using fork()
    - then the child process uses execvp to execute ./process
    - the parent process uses the kill function to send SIGINT (which is ctrl + c)
        to terminate the child process after sleeping for a few (3) seconds
    */
    
    int child_pid = fork();

    // child process
    if (child_pid == 0) {

        char * exec_arr[2];
        exec_arr[0] = "./process";
        exec_arr[1] = NULL;

        execvp(exec_arr[0], exec_arr);

        // printf("CHILD PID: %d\n", getpid());
    }

    // parent process
    else {
        // printf("CHILD PID (BY PARENT): %d\n", child_pid);
        
        printf("KILLING CHILD...\n");
        sleep(3);

        if (kill(child_pid, SIGINT)) {
            fprintf(stderr, "terminate of %d failed", child_pid);
        }

        printf("DEAD.\n");
    }

    return 0;
}