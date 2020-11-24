#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>

// global struct
struct information {
    int s_id_available; // student id available to generate
    int student_count;
    int total_students;
    pthread_mutex_t lock;
    pthread_cond_t queue_empty;
    pthread_cond_t queue_full;
};

void * teacher_routine(void * arg) {

    struct information * info = (struct information *) arg;
    while (info->student_count < info->total_students) {
        pthread_mutex_lock(&info->lock);
        
        if (info->s_id_available == 1) 
            pthread_cond_wait(&info->queue_empty, &info->lock);
        
        assert(info->s_id_available == 0);
        
        info->student_count++;
        printf("Teacher: student id %d for the next student.\n", info->student_count);
        info->s_id_available = 1;
        pthread_cond_signal(&info->queue_full);
        pthread_mutex_unlock(&info->lock);
    }

    pthread_exit(0);
}

void * student_routine(void * arg) {

    struct information * info = (struct information *) arg;

    pthread_mutex_lock(&info->lock);
    while (info->s_id_available == 0) {
        pthread_cond_wait(&info->queue_full, &info->lock);
    }
        
    assert(info->s_id_available == 1);

    printf("Student: My student id is %d.\n", info->student_count);
    info->s_id_available = 0;
    pthread_cond_signal(&info->queue_empty);
    pthread_mutex_unlock(&info->lock);

    pthread_exit(0);
}

int main(int argv, char* args[]) {

    // user inputs
    int n;
    printf("Number of students: ");
    scanf("%d", &n);
    printf("\n");

    // initialise thread ids and attributes
    pthread_t teacher_tid;
    pthread_t tids[n];
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    // initialise struct
    struct information info;
    info.s_id_available = 0;
    info.student_count = 0;
    info.total_students = n;
    pthread_mutex_init(&info.lock, NULL);
    pthread_cond_init(&info.queue_empty, NULL);
    pthread_cond_init(&info.queue_full, NULL);

    // create teacher routine
    pthread_create(&teacher_tid, &attr, teacher_routine, &info);

    // create student routines
    for (int i = 0; i < n; i++) {
        pthread_create(&tids[i], &attr, student_routine, &info);
    }

    // join teacher thread
    pthread_join(teacher_tid, NULL);

    // join student threads
    for (int i = 0; i < n; i++) {
        pthread_join(tids[i], NULL);
        // printf("Thread %d is still running.\n", i);
    }

    pthread_attr_destroy(&attr);
    pthread_mutex_destroy(&info.lock);
    pthread_cond_destroy(&info.queue_empty);
    pthread_cond_destroy(&info.queue_full);

    printf("\nMain thread: All students have done their oral examinations. The simulation will end now.\n");

    return 0;
}

