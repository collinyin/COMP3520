/*
    COMP3520 Exercise 3 - FCFS Dispatcher

    usage:

        ./fcfs <TESTFILE>
        where <TESTFILE> is the name of a job list
*/

/* Include files */
#include "fcfs.h"

void print_linked_list(MabPtr arena) {
    MabPtr temp_m;
    
    printf("\nMemory Linked List\n");
        temp_m = arena;
        while (temp_m) {
            if (temp_m->next) printf("[Offset: %d, Size: %d, Alloc: %d] --> ", temp_m->offset, temp_m->size, temp_m->allocated);
            else printf("[Offset: %d, Size: %d, Alloc: %d] --> NULL\n", temp_m->offset, temp_m->size, temp_m->allocated);
            
            temp_m = temp_m->next;
        }
}

int main (int argc, char *argv[])
{
    /*** Main function variable declarations ***/

    FILE * input_list_stream = NULL;
    PcbPtr current_process = NULL;
    PcbPtr process = NULL;
    int timer = 0;
    int decrease_timer_by = 0;
    int total_num_of_processes = 0;
    int free_flag = FALSE;

    /*** Main function variable queue declarations (they are all FCFS queues) ***/

    PcbPtr job_dispatcher = NULL;
    PcbPtr rt_queue = NULL; 
    PcbPtr normal_queue = NULL;
    PcbPtr level_0_queue = NULL;
    PcbPtr level_1_queue = NULL;
    PcbPtr level_2_queue = NULL;

    /*** Main function variable memory allocation arena declaration ***/

    MabPtr arena = NULL;

//  2. Create Arena
    arena = (MabPtr) malloc(sizeof(Mab));
    
    if (!arena) {
        fprintf(stderr,"memory allocation error\n");
        exit(127);
    }
    arena->offset = 0;
    arena->size = MEMORY_SIZE;
    arena->allocated = FALSE;
    arena->next = NULL;
    arena->prev = NULL;
    
//  3. Populate the job_dispatcher
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

//  put processes into job_dispatcher
    while (!feof(input_list_stream)) {
        process = createnullPcb();
        if (fscanf(input_list_stream,"%d, %d, %d, %d",
            &(process->arrivaltime), 
            &(process->servicetime),
            &(process->priority),
            &(process->mbytes)) != 4) {
            free(process);
            continue;
        }

        process->remainingcputime = process->servicetime;
        process->status = PCB_INITIALIZED;
        job_dispatcher = enqPcb(job_dispatcher, process);
        total_num_of_processes++;
    }
    
//  Declare 2 arrays that contain each processes turnaround and wait times
    float turnaround_time_arr[total_num_of_processes];
    float wait_time_arr[total_num_of_processes];
    int arr_indexer = 0;

//  4. Ask the user to enter an integer value for 'time_quantum'
    
    int time_quantum;
    printf("Enter Time Quantum (int): ");
    scanf("%d", &time_quantum);

//  5. Whenever there is a running process or queues aren't empty:
    PcbPtr temp_p;

    while (current_process || rt_queue || normal_queue || job_dispatcher
            || level_0_queue || level_1_queue || level_2_queue)
    {

//      i. Unload all arrived processes from job dispatcher into RT or Normal dispatch Q
        while (job_dispatcher) {
            if (timer >= job_dispatcher->arrivaltime) {
                temp_p = deqPcb(&job_dispatcher);
//              a. if job belongs in rt_queue
                if (temp_p->priority == 0)
                    rt_queue = enqPcb(rt_queue, temp_p);
//              b. if job belongs in normal_queue
                else if (temp_p->priority == 1)
                    normal_queue = enqPcb(normal_queue, temp_p);
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
//              A. Terminate the process
                terminatePcb(current_process);
//              B. Calculate the turnaround time for the process
                turnaround_time_arr[arr_indexer] = timer - current_process->arrivaltime;
                wait_time_arr[arr_indexer] = turnaround_time_arr[arr_indexer] - current_process->servicetime;
                arr_indexer++;
//              C. Deallocate the PCB (process control block)'s memory
                memFree(current_process->memoryblock);
                free(current_process);
                current_process = NULL;
//              D. Mark that a process has been terminated
                free_flag = TRUE;
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

                    memFree(current_process->memoryblock);
                    free(current_process);
                    current_process = NULL;
//                  Mark that a process has been terminated
                    free_flag = TRUE;
                }

//              C. Timer has NOT been exhausted:
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
//              A. decrease remaining_cpu_time by 1
                current_process->remainingcputime -= 1;

//              B. If time has been exhausted
                if (current_process->remainingcputime <= 0) {

                    terminatePcb(current_process);

                    turnaround_time_arr[arr_indexer] = timer - current_process->arrivaltime;
                    wait_time_arr[arr_indexer] = turnaround_time_arr[arr_indexer] - current_process->servicetime;
                    arr_indexer++;

                    memFree(current_process->memoryblock);
                    free(current_process);
                    current_process = NULL;
//                  Mark that a process has been terminated
                    free_flag = TRUE;       
                }

//              C. if there is a process with priority 0 waiting
                else if (level_0_queue) {
                    temp_p = suspendPcb(current_process);
                    level_2_queue = enqPcbHd(level_2_queue, temp_p);
                    current_process = deqPcb(&level_0_queue);
                    startPcb(current_process);
                }
                
//              D. if there is a process with priority 1 waiting
                else if (level_1_queue) {
                    temp_p = suspendPcb(current_process);
                    level_2_queue = enqPcbHd(level_2_queue, temp_p);
                    current_process = deqPcb(&level_1_queue);
                    startPcb(current_process); 
                }

//              E. There are no level 0 or level 1 processes waiting
                else {
                    if (current_process->remainingcputime <= 0) {
                        terminatePcb(current_process);

                        turnaround_time_arr[arr_indexer] = timer - current_process->arrivaltime;
                        wait_time_arr[arr_indexer] = turnaround_time_arr[arr_indexer] - current_process->servicetime;
                        arr_indexer++;

                        memFree(current_process->memoryblock);
                        free(current_process);
                        current_process = NULL;
//                      Mark that a process has been terminated
                        free_flag = TRUE;
                    }
//                  Continue running if no other processes are waiting
                }
            }
        }

//      iiia. Unload admittable jobs from rt_queue into level_0_queue 
        while (rt_queue) {  
//          check if there is a fit
            rt_queue->memoryblock = memAlloc(arena, rt_queue->mbytes);
            if (rt_queue->memoryblock) {
                temp_p = deqPcb(&rt_queue);
                level_0_queue = enqPcb(level_0_queue, temp_p);
            }
            else {
                break;
            }
        }

//      iiib. Unload admittable jobs from normal_queue into level_1_queue 
        while (normal_queue) {    
//          check if there is a fit
            normal_queue->memoryblock = memAlloc(arena, normal_queue->mbytes);
            if (normal_queue->memoryblock) {
                temp_p = deqPcb(&normal_queue);
                level_1_queue = enqPcb(level_1_queue, temp_p);
            }
            else {
                break;
            }
        }

//      iv. If no process was freed during this iteration
        if (free_flag == FALSE) {

//          a. If there is no running process and there is a process ready to run
//                  - run the process with highest priority
            if (!current_process && (level_0_queue || level_1_queue || level_2_queue))
            {
                if (level_0_queue) {
                    current_process = deqPcb(&level_0_queue);
                }
                else if (level_1_queue) {
                    current_process = deqPcb(&level_1_queue);
                }
                else {
                    current_process = deqPcb(&level_2_queue);
                }

                // print_linked_list(arena);
                startPcb(current_process);
            }

//          b.  Let the dispatcher sleep for the correct time & increment dispatcher's timer
//              Sleeps only when process exists (to prevent a seg fault when checking for remainingcputime)
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

//          c. If no process exists but there are jobs in the queues let the dispatcher sleep for 1
            else {
//              If all jobs have finished and there are none left in any queues 
//                  we break the loop so the main thread doesn't sleep for quantum again
                if (!current_process && !level_0_queue && !level_1_queue && !level_2_queue
                        && !rt_queue && !normal_queue && !job_dispatcher) 
                    break;

                sleep(1);
                timer++;
            }
        }

//      v. Unflag free_flag
        free_flag = FALSE;

//      vi. Go back to 4.
    }

//  6a. Calculate the average turnaround time
    float tot_turnaround = 0;
    float avg_turnaround = 0;
    int i;
    for (i = 0; i < total_num_of_processes; i++) {
        tot_turnaround += turnaround_time_arr[i];
    }
    avg_turnaround = tot_turnaround/total_num_of_processes;
    printf("\nAverage Turnaround Time: %.2f\n", avg_turnaround);

//  6b. Calculate the average wait time
    float tot_wait = 0;
    float avg_wait = 0;
    for (i = 0; i < total_num_of_processes; i++) {
        tot_wait += wait_time_arr[i];
    }
    avg_wait = tot_wait/total_num_of_processes;
    printf("Average Wait Time: %.2f\n\n", avg_wait);

//  7. Terminate the dispatcher
    exit(EXIT_SUCCESS);
}