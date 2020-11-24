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

    /*** Main function variable declarations for Assignment 2 ***/
    PcbPtr rrb_queue = NULL;
    int total_num_of_processes = 0;
    
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
        total_num_of_processes++;
    }
    
//  Declare 2 arrays that contain each processes turnaround and wait times
    float turnaround_time_arr[total_num_of_processes];
    float wait_time_arr[total_num_of_processes];
    int arr_indexer = 0;

//  3. Ask the user to enter an integer value for 'time_quantum'

    int time_quantum;
    printf("Enter Time Quantum (int): ");
    scanf("%d", &time_quantum);

//  4. Whenever there is a running process or EITHER queue is not empty:

    PcbPtr temp_p;
    while (current_process || fcfs_queue || rrb_queue)
    {
//      i. Unload arrived pending processes from fcfs_queue into rrb_queue 
//      (i.e. dequeue process from fcfs_queue and enqueue it onto rrb_queue)
        
        while (fcfs_queue) {    
            if (timer >= fcfs_queue->arrival_time) {
                    temp_p = deqPcb(&fcfs_queue);
                    rrb_queue = enqPcb(rrb_queue, temp_p);
            }
            else {
                break;
            }
        }

//      ii. If there is a process currently running;
        if (current_process)
        {
//          a. Decrement the process's remaining_cpu_time variable by time_quantum;
            current_process->remaining_cpu_time -= time_quantum;
//          b. If the process's allocated time has expired:
            if (current_process->remaining_cpu_time <= 0)
            {
//              A. Terminate the process;
                terminatePcb(current_process);
                
//              i. Calculate the turnaround time for the process
                turnaround_time_arr[arr_indexer] = timer - current_process->arrival_time;
                wait_time_arr[arr_indexer] = turnaround_time_arr[arr_indexer] - current_process->service_time;
                arr_indexer++;

//              B. Deallocate the PCB (process control block)'s memory
                free(current_process);
                current_process = NULL;
            }

//          c. Else if other processes are waiting in rrb_queue
            else if (rrb_queue) 
            {
//              A. Suspend currently running process
                temp_p = suspendPcb(current_process);
//              B. Enqueue it back to the tail of rrb_queue
                rrb_queue = enqPcb(rrb_queue, temp_p);
                current_process = NULL;
            }
//          d. if there are no other processes in rrb_queue then we continue running current process
        }

//      iii. If there is no running process and there is a process ready to run:
        if (!current_process && rrb_queue)
        {
//          Dequeue the process at the head of the queue, and start or continue the process
            current_process = deqPcb(&rrb_queue);
            startPcb(current_process);  
        }

//      iv. Let the dispatcher sleep for time_quantum & increment dispatcher's timer
//          Sleeps only when process exists (to prevent a seg fault when checking for remaining_cpu_time)
        if (current_process) {
//          if the time left on process is less than time quantum then we sleep for remaining time            
            if (current_process->remaining_cpu_time < time_quantum) {
                sleep(current_process->remaining_cpu_time);
                timer += (current_process->remaining_cpu_time);
            }
            else {
                sleep(time_quantum);
                timer += time_quantum;
            }
        }

//          If no process exists but there are jobs in the queues
        else {
//          If all jobs have finished and there are none left in any queues 
//              we break the loop so the main thread doesn't sleep for quantum again
            if (!current_process && !fcfs_queue && !rrb_queue) 
                break;

            sleep(1);
            timer++;
        }

//      v. Go back to 4.
    }

//  Calculate the average turnaround time for the round robin queue
    float tot_turnaround = 0;
    float avg_turnaround = 0;
    int i;
    for (i = 0; i < total_num_of_processes; i++) {
        tot_turnaround += turnaround_time_arr[i];
    }
    avg_turnaround = tot_turnaround/total_num_of_processes;
    printf("\nAverage Turnaround Time: %.2f\n", avg_turnaround);

//  Calculate the average wait time for the round robin queue
    float tot_wait = 0;
    float avg_wait = 0;
    for (i = 0; i < total_num_of_processes; i++) {
        tot_wait += wait_time_arr[i];
    }
    avg_wait = tot_wait/total_num_of_processes;
    printf("Average Wait Time: %.2f\n\n", avg_wait);

//  5. Terminate the RRB dispatcher
    exit(EXIT_SUCCESS);
}