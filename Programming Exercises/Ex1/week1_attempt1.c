#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// global struct
struct information {
    int teacher_exists;
    int student_count;
    int total_num_threads;
    pthread_mutex_t mutex;
};

void student_routine(int id, struct information * arg) {
    
    struct information * info_ptr = arg;

    printf("Student: My student id is %d.\n", info_ptr->student_count);
    info_ptr->student_count++;
    pthread_mutex_unlock(&info_ptr->mutex);
}

void * teacher_routine(void * arg) {
    
    struct information * info_ptr = (struct information *) arg;

    if (info_ptr->teacher_exists == 0) {
        info_ptr->teacher_exists = 1;
    }
    else {
        pthread_mutex_lock(&info_ptr->mutex);
        printf("Teacher: student id %d for next student.\n", info_ptr->student_count);
        student_routine(info_ptr->student_count, info_ptr);
    }

    pthread_exit(0);
}

int main(int argc, char * argv[]) {

    // get total number of students
    int numOfStudents;
    printf("Enter an integer: ");
    int err = scanf("%d", &numOfStudents);

    // error handling
    if (!err) {
        printf("\nA non integer was entered.\n");
        printf("Exiting...\n");
        exit(-1);
    }
    if (numOfStudents < 1) {
        exit(-1);
    }

    // initialise the global struct
    struct information info;
    info.teacher_exists = 0;
    info.student_count = 0;
    info.total_num_threads = numOfStudents + 1;
    // initialise the mutex
    pthread_mutex_init(&info.mutex, NULL);

    // create threads
    pthread_t tids[info.total_num_threads];

    for (int i = 0; i < info.total_num_threads; i++) {
        pthread_attr_t attr;
        pthread_attr_init(&attr);

        pthread_create(&tids[i], &attr, teacher_routine, &info);
    }

    for (int i = 0; i < info.total_num_threads; i++) {
        pthread_join(tids[i], NULL);
    }

    // when all students have received their IDs
    printf("All students have obtained their student IDs. The simulation will end now.\n");
    pthread_mutex_destroy(&info.mutex);

    return 0;
}