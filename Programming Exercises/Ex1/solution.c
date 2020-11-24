#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>

void * teacher_routine(void *);
void * students_routine(void *);

//declare global variables
int n; //total number of students

int s_id; //student id buffer
int s_id_available;

//declare global mutex and condition variables
pthread_mutex_t cond_queue_lock;

pthread_cond_t cond_queue_empty;
pthread_cond_t cond_queue_full;


int main(int argc, char ** argv)
{
    pthread_t *student_thrd_ids, teacher_thrd_id; //system thread id
    int *students; //user-defined thread id

    int i, rc;

    // input parameters
    // ask for the total number of students.
    printf("Enter the total number of students n (int): ");
    scanf("%d", &n);

    //allocate memory for students 
    student_thrd_ids = malloc(n * sizeof(pthread_t)); //system thread ids
    if (student_thrd_ids == NULL)
    {
        fprintf(stderr, "student_thrd_ids out of memory\n");
        exit(1);
    }

    //initialize global variable
    s_id_available = 0;

    //initialize mutex and condition variables
    pthread_cond_init (&cond_queue_empty, NULL);
    pthread_cond_init (&cond_queue_full, NULL);
    pthread_mutex_init (&cond_queue_lock, NULL);

    //create teacher thread 
    rc = pthread_create(&teacher_thrd_id, NULL, teacher_routine, NULL);
    if (rc)
    {
        printf("ERROR; return code from pthread_create() (teacher_routine) is %d\n", rc);
        exit(-1);
    }
	
    //create student threads 
    for (i = 0; i<n; i++)
    {
        rc = pthread_create(&student_thrd_ids[i], NULL, students_routine, NULL);
	if (rc) 
        {
	    printf("ERROR; return code from pthread_create() (vehicle) is %d\n", rc);
	    exit(-1);
	}
    }
    
    //join students and teacher threads.
    for (i = 0; i<n; i++) 
    {
	pthread_join(student_thrd_ids[i], NULL);
    }
    pthread_join(teacher_thrd_id, NULL);
    printf("Main thread: All student threads have obtained their student ids. The simulation will end now.\n");

    //deallocate allocated memory
    //free(student_thrd_ids);
 
    //destroy mutex and condition variable objects
    pthread_mutex_destroy(&cond_queue_lock);
    pthread_cond_destroy(&cond_queue_empty);
    pthread_cond_destroy(&cond_queue_full);	
	
    exit(0);
}

void * teacher_routine(void * arg)
{
    int l_id = 0;

    //assign ids to students
    while (l_id < n)
    {
        l_id++; //next student id 

        pthread_mutex_lock(&cond_queue_lock); 
        if (s_id_available == 1)
            pthread_cond_wait(&cond_queue_empty, &cond_queue_lock); 

        assert(s_id_available == 0); // to check for an error (program shouldn't continue if true).

        s_id = l_id; //place the id to the buffer   
        printf("Teacher: student id %d for next student.\n", l_id);

        s_id_available = 1; 
        pthread_cond_signal(&cond_queue_full); 
        pthread_mutex_unlock(&cond_queue_lock); 
    }

    pthread_exit(EXIT_SUCCESS);
}

void * students_routine(void * arg)
{
    int my_id;

    //obtain my student id from teacher	
    pthread_mutex_lock(&cond_queue_lock); 
    while (s_id_available == 0) 
        pthread_cond_wait(&cond_queue_full, &cond_queue_lock); 

    assert(s_id_available == 1); // to check for an error (program shouldn't continue if true).

    my_id = s_id; //take the student id from the buffer
    printf("Student: My student id is %d.\n", my_id);

    s_id_available = 0; 
    pthread_cond_signal(&cond_queue_empty); 
    pthread_mutex_unlock(&cond_queue_lock); 

    pthread_exit(EXIT_SUCCESS);
}

