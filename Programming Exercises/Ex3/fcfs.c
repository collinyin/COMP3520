/*
    COMP3520 Exercise 3 - FCFS Dispatcher

    usage:

        ./fcfs <TESTFILE>
        where <TESTFILE> is the name of a job list
*/

/* Include files */
#include "fcfs.h"

int main (int argc, char *argv[])
{
    /*** Main function variable declarations ***/

    FILE * input_list_stream = NULL;
    PcbPtr fcfs_queue = NULL;
    PcbPtr current_process = NULL;
    PcbPtr process = NULL;
    int timer = 0;

//  1. Populate the FCFS queue

    if (argc <= 0)
    {
        fprintf(stderr, "FATAL: Bad arguments array\n");
        exit(EXIT_FAILURE);
    }
    else if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <TESTFILE>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (!(input_list_stream = fopen(argv[1], "r")))
    {
        fprintf(stderr, "ERROR: Could not open \"%s\"\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    while (!feof(input_list_stream)) {  // put processes into fcfs_queue
        process = createnullPcb();
        if (fscanf(input_list_stream,"%d, %d",
             &(process->arrival_time), 
             &(process->service_time)) != 2) {
            free(process);
            continue;
        }
	process->remaining_cpu_time = process->service_time;
        process->status = PCB_INITIALIZED;
        fcfs_queue = enqPcb(fcfs_queue, process);
    }


//  2. Whenever there is a running process or the FCFS queue is not empty:

    while (current_process || fcfs_queue)
    {

        // printf("_____HEAD OF FCFS QUEUE_____\n");
        // printPcb(fcfs_queue);

//      i. If there is a currently running process;
        if (current_process)
        {
//          a. Decrement the process's remaining_cpu_time variable;
            current_process->remaining_cpu_time-=2;
            
//          b. If the process's allocated time has expired:
            if (current_process->remaining_cpu_time <= 0)
            {
//              A. Terminate the process;
                terminatePcb(current_process);
                
//              B. Deallocate the PCB (process control block)'s memory
                free(current_process);
                current_process = NULL;
            }
        }

//      ii. If there is no running process and there is a process ready to run:
        if (!current_process && fcfs_queue && fcfs_queue->arrival_time <= timer)
        {
//          Dequeue the process at the head of the queue, set it as currently running and start it
            current_process = deqPcb(&fcfs_queue);
            startPcb(current_process);
        }
        
//      iii. Let the dispatcher sleep for one second;
        sleep(2);
        
//      iv. Increment the dispatcher's timer;
        timer+=2;
        
//      v. Go back to 2.
        printf("GOING BACK UP!\n");
    }
    
//  3. Terminate the FCFS dispatcher
    exit(EXIT_SUCCESS);
}
