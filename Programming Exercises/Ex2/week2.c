#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include <time.h>
#include <unistd.h>

int s_id_available;
int this_student_inside;
int student_count;
int total_students;
float max_time;
float time_taken;
pthread_mutex_t lock;

pthread_attr_t attr;

pthread_cond_t teacher_waiting;
pthread_cond_t students_waiting;
pthread_cond_t queue_empty;
pthread_cond_t queue_full;
pthread_cond_t room_full;
pthread_cond_t room_empty;

// global student struct
struct student {
    int student_id;
};

void * teacher_routine(void * arg) {
    
    // Give out student IDs
    while (student_count < total_students) {
        pthread_mutex_lock(&lock);
        
        if (s_id_available == 1) {
            int r = pthread_cond_wait(&queue_empty, &lock);
            if (r != 0)
                printf("Error %d\n", r);
        }
            
        assert(s_id_available == 0);

        student_count++;
        printf("Teacher: student id %d for the next student.\n", student_count);
        s_id_available = 1;
        pthread_cond_signal(&queue_full);
        pthread_mutex_unlock(&lock);
    }

    pthread_mutex_lock(&lock);
    pthread_cond_wait(&teacher_waiting, &lock);
    printf("\nRELEASE ALL STUDENTS.\n\n");
    pthread_cond_broadcast(&students_waiting);
    pthread_mutex_unlock(&lock);

    sleep(1);
    student_count = 0;

    // Call out student IDs for oral test
    while (student_count < total_students + 1) {
        pthread_mutex_lock(&lock);
        if (this_student_inside == 1) {
            pthread_cond_wait(&room_full, &lock);
        }

        assert(this_student_inside == 0);
        if (student_count != 0)
            printf("Teacher: Student %d took %f units to complete the exam.\n", student_count, time_taken);
        
        student_count++;

        // to print out the last line
        if (student_count > total_students) 
            pthread_exit(0);
            
        printf("_______________________________________________________________________________\n");
        printf("Teacher: The office is now available. Student %d can enter.\n", student_count);
        this_student_inside = 1;
        pthread_cond_broadcast(&room_empty);
        pthread_mutex_unlock(&lock);

    }

    pthread_exit(0);
}

void * student_routine(void * arg) {
    
    // Take student IDs
    pthread_mutex_lock(&lock);
    while (s_id_available == 0) {
        pthread_cond_wait(&queue_full, &lock);
    }
        
    assert(s_id_available == 1);

    ((struct student *) arg)->student_id = student_count;
    printf("Student: My student id is %d.\n", ((struct student *) arg)->student_id);
    s_id_available = 0;
    pthread_cond_signal(&queue_empty);
    pthread_mutex_unlock(&lock);

    // wait until the last student, 
    // then signal teacher that all students have received their ID
    
    pthread_mutex_lock(&lock);
    if (((struct student *) arg)->student_id == total_students) {
        pthread_cond_signal(&teacher_waiting);  
    }
    pthread_cond_wait(&students_waiting, &lock);
    pthread_mutex_unlock(&lock);
    
    // to prevent a race condition
    sleep(1);

    // Go into classroom for oral test
    pthread_mutex_lock(&lock);
    while (((struct student *) arg)->student_id != student_count) {
        pthread_cond_wait(&room_empty, &lock);
    }
    
    assert(this_student_inside == 1);
    
    printf("\nStudent %d: My id is called by the teacher and I will enter the office now.\n", ((struct student *) arg)->student_id);

    this_student_inside = 0;
    time_taken = (float)rand()/(float)(RAND_MAX/(max_time+3));
    printf("I TOOK %f UNITS\n", time_taken);
    if (time_taken < max_time/2) {
        time_taken = max_time/2;
    }
    if (time_taken > max_time) {
        time_taken = max_time;
    }
    sleep(time_taken);
    printf("Student %d: I have completed the exam and leave the room now.\n\n", ((struct student *) arg)->student_id);

    pthread_cond_signal(&room_full);
    pthread_mutex_unlock(&lock);

    free(arg);
    pthread_exit(0);
}

int main(int argv, char* args[]) {

    // user inputs
    int n;
    printf("Number of students: ");
    scanf("%d", &n);

    int t;
    printf("Maximum time (T): ");
    scanf("%d", &t);
    printf("\n");

    // initialise thread ids and attributes
    pthread_t teacher_tid;
    pthread_t tids[n];
    
    pthread_attr_init(&attr);

    // initialise global variables
    s_id_available = 0;
    student_count = 0;
    this_student_inside = 0;
    total_students = n;
    max_time = t;
    pthread_mutex_init(&lock, NULL);

    pthread_cond_init(&teacher_waiting, NULL);
    pthread_cond_init(&students_waiting, NULL);
    pthread_cond_init(&queue_empty, NULL);
    pthread_cond_init(&queue_full, NULL);
    pthread_cond_init(&room_full, NULL);
    pthread_cond_init(&room_empty, NULL);

    
    // create teacher routine
    pthread_create(&teacher_tid, &attr, teacher_routine, NULL);

    // create student routines
    for (int i = 0; i < n; i++) {
        struct student * st = (struct student *) malloc (sizeof(struct student));
        pthread_create(&tids[i], &attr, student_routine, (void *) st);
    }

    // join teacher thread
    pthread_join(teacher_tid, NULL);

    // join student threads
    for (int i = 0; i < n; i++) {
        pthread_join(tids[i], NULL);
    }

    pthread_attr_destroy(&attr);

    pthread_mutex_destroy(&lock);

    pthread_cond_destroy(&teacher_waiting);
    pthread_cond_destroy(&students_waiting);
    pthread_cond_destroy(&queue_empty);
    pthread_cond_destroy(&queue_full);
    pthread_cond_destroy(&room_full);
    pthread_cond_destroy(&room_empty);

    printf("_______________________________________________________________________________\n");
    printf("Main thread: All students have done their oral examinations. The simulation will end now.\n");

    return 0;
}