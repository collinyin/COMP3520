#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include <time.h>
#include <unistd.h>

/*____________________________GLOBAL VARIABLES____________________________*/

// user inputs
int N; // total number of students
int K; // number of students in each group
float T; // the time limit for each group

// synchronisation
pthread_mutex_t lock;
pthread_mutex_t last_lock;

pthread_attr_t attr;

pthread_cond_t group_not_full;
pthread_cond_t group_full;
pthread_cond_t student_waiting;
pthread_cond_t teacher_waiting;
pthread_cond_t room_available;
pthread_cond_t group_ready;
pthread_cond_t leave_room;
pthread_cond_t last_student;

// general
int ids_available;
int group_inside;
int num_of_groups;
int group_counter;
int group_student_counter;
int total_student_counter;
float time_taken;
int last_student_check;

// student struct
struct student {
    int groupID;
};

void * teacher_routine(void * arg) {

          /*__________________________________STAGE 1: Distribute all student IDs_________________________________*/
    printf("__________________________________STAGE 1: Distribute all student IDs_________________________________\n\n");

    while (total_student_counter < N) {
        pthread_mutex_lock(&lock);

        // teacher waits until students have taken all the distributed IDs
        if (ids_available == 1) { // there currently are IDs available
            int r = pthread_cond_wait(&group_full, &lock);
            if (r != 0) {
                printf("ERROR %d for pthread_cond_wait\n", r);
            }
        }
        
        // all students have taken their IDs and now the teacher can distribute more
        assert(ids_available == 0);

        printf("Teacher: Now I start to assign group id %d to students.\n", group_counter);

        if (N - total_student_counter >= K) {
            group_student_counter = K;
            total_student_counter += K;
        }
        else {
            group_student_counter = N - total_student_counter;
            total_student_counter = N;
        }

        // teacher announces that IDs are now available for collection
        ids_available = 1;
        pthread_cond_broadcast(&group_not_full);

        pthread_mutex_unlock(&lock);
    }

    // Teacher waits until the last student has collected their ID
    pthread_mutex_lock(&lock);
    pthread_cond_wait(&teacher_waiting, &lock);
    // printf("\nRELEASE ALL STUDENTS\n\n");
    pthread_cond_broadcast(&student_waiting);
    pthread_mutex_unlock(&lock);

    // this sleep is to ensure that all student threads have finished before moving onto the next stage
    sleep(1);
    
            /*___________________________________STAGE 2: Call group IDs into Lab___________________________________*/
    printf("\n___________________________________STAGE 2: Call group IDs into Lab___________________________________\n\n");
    
    pthread_mutex_lock(&lock);
    group_counter = 1;
    group_student_counter = 0;
    total_student_counter = 0;
    pthread_mutex_unlock(&lock); 

    while (total_student_counter < N && group_counter) {
        pthread_mutex_lock(&lock);

        // teacher waits until all students in group have been signalled to enter the lab
        if (group_inside == 1) { // there currently are students who haven't been signalled
            int r = pthread_cond_wait(&group_ready, &lock);
            if (r != 0) {
                printf("ERROR %d for pthread_cond_wait\n", r);
            }
        }
        
        // teacher signals the next group
        assert(group_inside == 0);

        // this condition is to prevent it from printing for the first group, since there is no previous group
        if (group_counter != 1) {
            // previous group's times taken to finish the exercise
            sleep(time_taken);
            
            printf("Teacher: Group %d took %f units of time to complete the exercise.\n", group_counter - 1, time_taken);
            printf("______________________________________________________________________________________________________\n");            
        }
        
        if (N % K > 0) {
            if (group_counter <= num_of_groups) {
                printf("Teacher: The lab is now available. Students in group %d can enter the room and start your lab exercise.\n", group_counter);
                group_inside = 1;
                pthread_cond_broadcast(&room_available);
            }
        }
        else {
            if (group_counter < num_of_groups) {
                printf("Teacher: The lab is now available. Students in group %d can enter the room and start your lab exercise.\n", group_counter);
                group_inside = 1;
                pthread_cond_broadcast(&room_available);
            }
        }
        

        pthread_mutex_unlock(&lock);
    }

    pthread_exit(0);
}

void * student_routine(void * arg) {

    /*___________________________________STAGE 1: Distribute all student IDs___________________________________*/

    pthread_mutex_lock(&lock);

    // students wait until teacher announces that ids are now available for collection
    while (ids_available == 0) {
        pthread_cond_wait(&group_not_full, &lock);
        // as soon as a signal is sent by the teacher, only one student thread acquires the mutex 
        // and collects the ID, the rest wait again
    }

    // student comes and collects ID
    assert(ids_available == 1);

    ((struct student *) arg)->groupID = group_counter;
    printf("Student: I am assigned to group %d.\n", ((struct student *) arg)->groupID);
    group_student_counter--;
    
    // if the group is full then all distributed IDs have been collected and a signal is sent back to the teacher
    if (group_student_counter == 0) {
        ids_available = 0; // there are no more IDs available
        group_counter++;
        pthread_cond_signal(&group_full);
    }

    pthread_mutex_unlock(&lock);

    // after all the student has collected their ID they wait until all students have obtained IDs
    pthread_mutex_lock(&lock);
    if (total_student_counter == N) {
        pthread_cond_signal(&teacher_waiting);
    }
    pthread_cond_wait(&student_waiting, &lock);
    pthread_mutex_unlock(&lock);

    /*___________________________________STAGE 2: Call group IDs into Lab___________________________________*/

    pthread_mutex_lock(&lock);

    // Students keep waiting until their group ID is called
    while (((struct student *) arg)->groupID != group_counter || group_inside == 0) {
        pthread_cond_wait(&room_available, &lock);
    }

    assert(group_inside == 1);
    printf("Student in group %d: My group is called by the teacher and I will enter the lab now.\n", ((struct student *) arg)->groupID);
    
    group_student_counter++;
    total_student_counter++;

    // the last group
    if (N - (K * (group_counter - 1)) < K) {
        // edge case: when there are only 2 groups and when K > total number of students / 2
        if (K > N/2) {
            if (group_student_counter == (N % K)) {
                last_student_check = 1;
            }
        }
        else {
            if (group_student_counter == N % (group_counter - 1)) {
                last_student_check = 1;
            }
        }
        // the last student in the last group
        if (last_student_check == 1) {

            // +1 is an arbitrary number to simulate the time when it goes above T
            time_taken = (float)rand()/(float)(RAND_MAX/(T+1)); 
            // printf("Full time taken: %f\n", time_taken);

            if (time_taken < T/2) {
                time_taken = T/2;
            }
            if (time_taken > T) {
                time_taken = T;
            }

            group_inside = 0; // students finished the lab
            group_student_counter = 0;
            group_counter++;
            printf("Student in group %d: We have completed our exercise and leave the lab now.\n", ((struct student *) arg)->groupID);
            pthread_cond_signal(&group_ready);   
        }
    }
    else {
        // the last student in group (not the last group)
        if (group_student_counter == K) {
            
            // +1 is an arbitrary number to simulate the time when it goes above T
            time_taken = (float)rand()/(float)(RAND_MAX/(T+1)); 
            // printf("Full time taken: %f\n", time_taken);

            if (time_taken < T/2) {
                time_taken = T/2;
            }
            if (time_taken > T) {
                time_taken = T;
            }
            
            group_inside = 0; // students finished the lab
            group_student_counter = 0;
            group_counter++;
            printf("Student in group %d: We have completed our exercise and leave the lab now.\n", ((struct student *) arg)->groupID);
            pthread_cond_signal(&group_ready);
        }
    }

    pthread_mutex_unlock(&lock);

    free(arg);
    pthread_exit(0);
}

int main(int argv, char * args[]) {

    group_counter = 1;
    group_student_counter = 0;
    total_student_counter = 0;
    ids_available = 0;
    group_inside = 0;
    time_taken = 0;
    last_student_check = 0;

    // get user inputs
    int r;
    printf("______________________________________________________________________________________________________\n\n");
    printf("Enter number of students (N): ");
    r = scanf("%d", &N);
    if (r == 0) {
        printf("INVALID INPUT ENTERED.\n");
        printf("EXITING...\n");
        return -1;
    }
        
    printf("Enter number of students in each group (K): ");
    r = scanf("%d", &K);
    if (r == 0) {
        printf("INVALID INPUT ENTERED.\n");
        printf("EXITING...\n");
        return -1;
    }

    printf("Maximum time (T): ");
    r = scanf("%f", &T);
    if (r == 0) {
        printf("INVALID INPUT ENTERED.\n");
        printf("EXITING...\n");
        return -1;
    }
    printf("\n");

    // Error handling for invalid inputs
    if (N < 1 || K < 1 || T < 1) {
        printf("NEGATIVE / ZERO INPUT ENTERED.\n");
        printf("EXITING...\n");
        return -1;
    }

    if (N < K) {
        printf("THERE ARE MORE GROUPS THAN THE NUMBER OF STUDENTS.\n");
        printf("EXITING...\n");
        return -1;
    }

    // Getting the correct number of groups
    if (N > 1) {
        num_of_groups = N/K + 1;
    }
    else {
        num_of_groups = 1;
    } 

    // declaring/initialising synchronisation variables
    pthread_mutex_init(&lock, NULL);
    pthread_mutex_init(&last_lock, NULL);

    pthread_attr_init(&attr);

    pthread_cond_init(&group_not_full, NULL);
    pthread_cond_init(&group_full, NULL);
    pthread_cond_init(&teacher_waiting, NULL);
    pthread_cond_init(&student_waiting, NULL);
    pthread_cond_init(&room_available, NULL);
    pthread_cond_init(&group_ready, NULL);
    pthread_cond_init(&leave_room, NULL);
    pthread_cond_init(&last_student, NULL);

    pthread_t teacher_tid;
    pthread_t student_tids[N];

    // create teacher thread
    pthread_create(&teacher_tid, &attr, teacher_routine, NULL);

    // create student threads
    for (int i = 0; i < N; i++) {
        struct student * st = (struct student *) malloc (sizeof(struct student));
        pthread_create(&student_tids[i], &attr, student_routine, st);
    }

    // join teacher thread
    pthread_join(teacher_tid, NULL);

    // join student threads
    for (int i = 0; i < N; i++) {
        pthread_join(student_tids[i], NULL);
    }

    // destorying synchronisation variables
    pthread_mutex_destroy(&lock);
    pthread_mutex_destroy(&last_lock);

    pthread_attr_destroy(&attr);

    pthread_cond_destroy(&group_not_full);
    pthread_cond_destroy(&group_full);
    pthread_cond_destroy(&teacher_waiting);
    pthread_cond_destroy(&student_waiting);
    pthread_cond_destroy(&room_available);
    pthread_cond_destroy(&group_ready);
    pthread_cond_destroy(&leave_room);
    pthread_cond_destroy(&last_student);

    printf("\nMain thread: All students have completed their lab exercises. The simulation will end now.\n");

    return 0;
}