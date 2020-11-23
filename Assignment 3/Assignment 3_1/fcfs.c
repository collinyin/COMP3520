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
    PcbPtr current_process = NULL;
    PcbPtr process = NULL;
    int timer = 0;
    int decrease_timer_by = 0;
    int total_num_of_processes = 0;

    /*** Main function variable queue declarations (they are all FCFS queues) ***/
    PcbPtr rt_queue = NULL; 
    PcbPtr normal_queue = NULL;
    PcbPtr level_0_queue = NULL;
    PcbPtr level_1_queue = NULL;
    PcbPtr level_2_queue = NULL;

//  2. Populate the rt_queue and normal_queue

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

    //  put processes into rt_queue or normal_queue
    while (!feof(input_list_stream)) {
        process = createnullPcb();
        if (fscanf(input_list_stream,"%d, %d, %d",
            &(process->arrivaltime), 
            &(process->servicetime),
            &(process->priority)) != 3) {
            free(process);
            continue;
        }

        process->remainingcputime = process->servicetime;
        process->status = PCB_INITIALIZED;
        
        // Process belongs in rt_queue
        if (process->priority == 0) {
            rt_queue = enqPcb(rt_queue, process);
        }

        // Process belongs in normal_queue
        // - else statement because we are assuming valid inputs
        else {
            normal_queue = enqPcb(normal_queue, process);
        }

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
    while (current_process || rt_queue || normal_queue
            || level_0_queue || level_1_queue || level_2_queue)
    {
//      ia. Unload arrived pending processes from rt_queue into level_0_queue 
//          and normal_queue into level_1_queue  
        while (rt_queue) {    
            if (timer >= rt_queue->arrivaltime) {
                temp_p = deqPcb(&rt_queue);
                level_0_queue = enqPcb(level_0_queue, temp_p);
            }
            else {
                break;
            }
        }
//      ib. Unload arrived pending processes from normal_queue into level_1_queue 
        while (normal_queue) {    
            if (timer >= normal_queue->arrivaltime) {
                temp_p = deqPcb(&normal_queue);
                level_1_queue = enqPcb(level_1_queue, temp_p);
            }
            else {
                break;
            }
        }

//      ii. If there is a process currently running;
        if (current_process)
        {
//          a. If the priority value of process is 0
            if (current_process->priority == 0) {
//              A. set decrease_time_by to servicetime
                // decrease_timer_by = current_process->remainingcputime;

//              B. Terminate the process
                terminatePcb(current_process);
//              C. Calculate the turnaround time for the process
                turnaround_time_arr[arr_indexer] = timer - current_process->arrivaltime;
                wait_time_arr[arr_indexer] = turnaround_time_arr[arr_indexer] - current_process->servicetime;
                arr_indexer++;
//              D. Deallocate the PCB (process control block)'s memory
                free(current_process);
                current_process = NULL;
            }

//          b. If the priority value of process is 1
            else if (current_process->priority == 1) {
//              A. decrease remaining_cpu_time by time_quantum
                current_process->remainingcputime -= time_quantum;

//              B. Timer has been exhausted:
//                  - terminate process, free and calculate times
                if (current_process->remainingcputime <= 0) {
                    terminatePcb(current_process);

                    turnaround_time_arr[arr_indexer] = timer - current_process->arrivaltime;
                    wait_time_arr[arr_indexer] = turnaround_time_arr[arr_indexer] - current_process->servicetime;
                    arr_indexer++;

                    free(current_process);
                    current_process = NULL;
                }

//              C. Timer has NOT been exhausted and there is another process waiting:
//                  - suspend process, set new priority and enqueue on tail
                else {
                    current_process->priority = 2;
                    temp_p = suspendPcb(current_process);
                    level_2_queue = enqPcb(level_2_queue, temp_p);
                    current_process = NULL;
                }

            }
//          c. If the priority value of process is 2
            else if (current_process->priority == 2) {
                // regardless of situation process always takes up 1 second
                current_process->remainingcputime -= 1;
//              A. if there is a process with priority 0 waiting
                if (level_0_queue) {
                    temp_p = suspendPcb(current_process);
                    level_2_queue = enqPcbHd(level_2_queue, temp_p);
                    current_process = deqPcb(&level_0_queue);
                    startPcb(current_process); 
                    // current_process->remainingcputime -= 1;
                }
                
//              B. if there is a process with priority 1 waiting
                else if (level_1_queue) {
                    temp_p = suspendPcb(current_process);
                    level_2_queue = enqPcbHd(level_2_queue, temp_p);
                    current_process = deqPcb(&level_1_queue);
                    startPcb(current_process); 
                    // current_process->remainingcputime -= 1;
                }

//              C. There are no level 0 or level 1 processes waiting
                else {
                    if (current_process->remainingcputime <= 0) {
                        terminatePcb(current_process);

                        turnaround_time_arr[arr_indexer] = timer - current_process->arrivaltime;
                        wait_time_arr[arr_indexer] = turnaround_time_arr[arr_indexer] - current_process->servicetime;
                        arr_indexer++;

                        free(current_process);
                        current_process = NULL;
                    }
//                  Continue running if no other processes are waiting
                }

//              D. Decrease timer by 1 regardless of situation
                // decrease_timer_by = 1;
            }
        }

//      iii. If there is no running process and there is a process ready to run
//              - run the process with highest priority
        if (!current_process && (level_0_queue || level_1_queue || level_2_queue))
        {
            if (level_0_queue) {
                current_process = deqPcb(&level_0_queue);
                startPcb(current_process);
            }
            else if (level_1_queue) {
                current_process = deqPcb(&level_1_queue);
                startPcb(current_process);
            }
            else {
                current_process = deqPcb(&level_2_queue);
                startPcb(current_process);
            }
        }

//      iv. Let the dispatcher sleep for the correct time & increment dispatcher's timer
//          Sleeps only when process exists (to prevent a seg fault when checking for remainingcputime)
        if (current_process) {
            
            if (current_process->priority == 0)
                decrease_timer_by = current_process->servicetime;

            if (current_process->priority == 1) {
                if (current_process->remainingcputime < time_quantum)
                    decrease_timer_by = current_process->remainingcputime;
                else
                    decrease_timer_by = time_quantum;
            }

            if (current_process->priority == 2)
                decrease_timer_by = 1;

            sleep(decrease_timer_by);
            timer += decrease_timer_by;
        }

//      v. If no process exists but there are jobs in the queues let the dispatcher sleep for 1
        else {
//          If all jobs have finished and there are none left in any queues 
//              we break the loop so the main thread doesn't sleep for quantum again
            if (!current_process && !level_0_queue && !level_1_queue && !level_2_queue
                    && !rt_queue && !normal_queue) 
                break;

            sleep(1);
            timer++;
        }

//      vi. Go back to 4.
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