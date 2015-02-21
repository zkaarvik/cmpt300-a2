#define _GNU_SOURCE

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sched.h>
#include <pthread.h>

#define NUM_SAMPLES_LARGE 10000000
#define NUM_SAMPLES_SMALL 1000000

unsigned long long timespecDiff(struct timespec *timeA_p, struct timespec *timeB_p)
{
    return ((timeA_p->tv_sec * 1000000000) + timeA_p->tv_nsec) -
           ((timeB_p->tv_sec * 1000000000) + timeB_p->tv_nsec); //Calculate difference between two times, returning the difference in ns
}

void set_cpu_affinity()
{
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(0, &mask);

    if(sched_setaffinity(0, sizeof(mask), &mask) < 0)
    {
       perror("Set CPU Affinity Error");
    }
    if(pthread_setaffinity_np(pthread_self(), sizeof(mask), &mask) < 0)
    {
       perror("Set CPU Affinity Error");
    }
}

void min_function() {}

double find_loop_overhead()
{
    struct timespec start, stop;
    int i;

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
    for (i = 0; i < NUM_SAMPLES_LARGE; i++)
    {

    }
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &stop);

    printf("Loop overhead: %f\n", (double) timespecDiff(&stop, &start) / NUM_SAMPLES_LARGE);
    return (double) timespecDiff(&stop, &start) / NUM_SAMPLES_LARGE;
}


void find_func_call_cost()
{
    struct timespec start, stop;
    unsigned long long total_time = 0;
    int i;

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
    for (i = 0; i < NUM_SAMPLES_LARGE; i++)
    {
        min_function();
    }
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &stop);
    total_time = timespecDiff(&stop, &start);

    printf("Function call with loop ovh: %f\n", (double) total_time / NUM_SAMPLES_LARGE);
    printf("Measured function call cost: %f\n", ((double) total_time / NUM_SAMPLES_LARGE) - find_loop_overhead());
}

void find_sys_call_cost()
{
    struct timespec start, stop;
    unsigned long long total_time = 0;
    int i;

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
    for (i = 0; i < NUM_SAMPLES_LARGE; i++)
    {
        getpid();
    }
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &stop);
    total_time = timespecDiff(&stop, &start);

    printf("System call with loop ovh: %f\n", (double) total_time / NUM_SAMPLES_LARGE);
    printf("Measured system call cost: %f\n", ((double) total_time / NUM_SAMPLES_LARGE) - find_loop_overhead());
}

void find_proc_switch_cost()
{
    pid_t child_pid;
    int pipefd1[2];
    int pipefd2[2];

    pipe(pipefd1);
    pipe(pipefd2);

    struct timespec start, stop;
    unsigned long long total_time = 0;

    //fork process to have a parent and child that will communicate
    child_pid = fork();
    if(child_pid == -1)
    {
        perror("Fork failure");
        exit(1);
    }

    if(child_pid == 0)
    {
        //running in child
        char readbuf;
        char writebuf = '1';

        //Close write side of first pipe, read side of second
        close(pipefd1[1]);
        close(pipefd2[0]);

        //continually read from input pipe
        //Then write back to output pipe
        //Read from pipefd1[0]
        while( read(pipefd1[0], &readbuf, sizeof(readbuf)) ){
            //Write to pipefd2[1]
            write(pipefd2[1], &writebuf, sizeof(writebuf));
        }

        close(pipefd1[0]);
        close(pipefd2[1]);

        exit(0);
    } 
    else
    {
        //running in parent
        char readbuf;
        char writebuf = '1';

        //Close read side of first pipe, write side of second
        close(pipefd1[0]);
        close(pipefd2[1]);

        clock_gettime(CLOCK_MONOTONIC, &start);

        int j;
        for(j = 0; j < NUM_SAMPLES_SMALL; j++)
        {
            //First write to pipe, then read
            //Write to pipefd1[1]
            write(pipefd1[1], &writebuf, sizeof(writebuf));

            //Read from pipefd2[0]
            read(pipefd2[0], &readbuf, sizeof(readbuf));
        }

        clock_gettime(CLOCK_MONOTONIC, &stop);
        total_time = timespecDiff(&stop, &start);

        close(pipefd1[1]);
        close(pipefd2[0]);

        //Double the number of samples as each counted for two context switches
        printf("Measured context switch cost: %f\n", (double) total_time / (NUM_SAMPLES_SMALL * 2));
    }
}

int c = 0;
int t1_lock = 0, t2_lock = 0;
pthread_mutex_t mutex;

void *fnC()
{
    while(c != 2)
    {
        if(t1_lock == 1)
        {
            pthread_mutex_lock(&mutex); 
            t2_lock = 1;
            c = 0;
        }

        if(t1_lock == 0)
        {
            pthread_mutex_unlock(&mutex);
            t2_lock = 0;
        }
    }    
}

void find_thread_switch_cost()
{
    int i;
    c = 0;

    struct timespec start, stop;
    unsigned long long total_time = 0;

    int rt1;
    pthread_t t1;

    //Create a child thread
    if( (rt1=pthread_create( &t1, NULL, &fnC, NULL)) )
    {
        printf("Thread creation failed: %d\n", rt1);
    }

    clock_gettime(CLOCK_MONOTONIC, &start);

    //Parent thread
    for(i = 0; i < NUM_SAMPLES_LARGE;)
    {
        if(t2_lock == 1)
        {
            pthread_mutex_lock(&mutex);
            t1_lock = 1;
            c = 1;
        }

        if(t2_lock == 0)
        {
            pthread_mutex_unlock(&mutex);
            t1_lock = 0;
            i++;
        }
    }
    c = 2;

    clock_gettime(CLOCK_MONOTONIC, &stop); 

    pthread_join(t1, NULL);

    total_time = timespecDiff(&stop, &start);
    printf("Measured thread switch cost: %f\n", (double) total_time / (NUM_SAMPLES_LARGE * 2));
}


int main()
{
    char user_choice;
    int i;

    set_cpu_affinity();

    while (1)
    {
        printf("\nChoose what to measure: \n");
        printf("1: Cost of a minimal function call\n");
        printf("2: Cost of a minimal system call\n");
        printf("3: Cost of process switching\n");
        printf("4: Cost of thread switching\n");
        printf("5: Exit\n\n");
        printf("Your choice?");

        user_choice = fgetc(stdin);
        while (user_choice != '\n' && fgetc(stdin) != '\n'); //Clear buffer of extra chars

        switch (user_choice)
        {
        case '1':
            find_func_call_cost();
            break;
        case '2':
            find_sys_call_cost();
            break;
        case '3':
            for(i=0;i<100;i++)
            {
            find_proc_switch_cost();
            usleep(10000);
            }
            break;
        case '4':
            for(i=0;i<100;i++)
            {
            find_thread_switch_cost();
            usleep(10000);
            }
            break;
        case '5':
            return 0;
        default:
            printf("Invalid choice...\n");
        }

        printf("Press Enter to Continue..");
        while ( getchar() != '\n' );
    }
}