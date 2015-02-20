#include <stdio.h>
#include <stdint.h>
#include <time.h>

#define NUM_SAMPLES 10000000

unsigned long long timespecDiff(struct timespec *timeA_p, struct timespec *timeB_p)
{
    return ((timeA_p->tv_sec * 1000000000) + timeA_p->tv_nsec) -
           ((timeB_p->tv_sec * 1000000000) + timeB_p->tv_nsec); //Calculate difference between two times, returning the difference in ns
}

void min_function() {}

//FN to find gettime overhead???
unsigned long long find_loop_overhead()
{
    struct timespec start, stop, dummy;

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);

    int i;
    for (i = 0; i < NUM_SAMPLES; i++)
    {
        //clock_gettime(CLOCK_MONOTONIC, &dummy);
        //clock_gettime(CLOCK_MONOTONIC, &dummy);
    }

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &stop);

    printf("DEBUG: LOOP OVH: %f\n", (double) timespecDiff(&stop, &start) / NUM_SAMPLES);
    return timespecDiff(&stop, &start) / NUM_SAMPLES;
}


void find_func_call_cost()
{
    struct timespec start, stop;
    unsigned long long total_time = 0;
    int i;

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
    for (i = 0; i < NUM_SAMPLES; i++)
    {
        min_function();

    }
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &stop);
    total_time = timespecDiff(&stop, &start);

    printf("DEBUG: W/O ACC FOR LOOP OVH: %f\n", (double) total_time / NUM_SAMPLES);
    printf("Measured function call cost: %f\n", ((double) total_time / NUM_SAMPLES) - find_loop_overhead());
}

void find_sys_call_cost()
{
    struct timespec start, stop;
    unsigned long long total_time = 0;
    int i;

    clock_gettime(CLOCK_MONOTONIC, &start);
    for (i = 0; i < NUM_SAMPLES; i++)
    {
        getpid();
    }
    clock_gettime(CLOCK_MONOTONIC, &stop);
    total_time = timespecDiff(&stop, &start);

    printf("DEBUG: W/O ACC FOR LOOP OVH: %f\n", (double) total_time / NUM_SAMPLES);
    printf("Measured function call cost: %f\n", ((double) total_time / NUM_SAMPLES) - find_loop_overhead());
}

void find_proc_switch_cost()
{

}

void find_thread_switch_cost()
{

}


int main()
{
    struct timespec start;
    struct timespec stop;
    unsigned long long result;
    char user_choice;

    while (1)
    {
        printf("Choose what to measure: \n");
        printf("1: Cost of a minimal function call\n");
        printf("2: Cost of a minimal system call\n");
        printf("3: Cost of process switching\n");
        printf("4: Cost of thread switching\n");
        printf("5: Exit\n\n");
        printf("Your choice?");
        //user_choice = getchar();
        //while(getchar() != '\n') //discard any other character on stdin until newline
        //scanf(" %c", &user_choice);
        user_choice = fgetc(stdin);
        while (user_choice != '\n' && fgetc(stdin) != '\n');

        switch (user_choice)
        {
        case '1':
            find_func_call_cost();
            break;
        case '2':
            find_sys_call_cost();
            break;
        case '3':
            find_proc_switch_cost();
            break;
        case '4':
            find_thread_switch_cost();
            break;
        case '5':
            find_loop_overhead();
            break;
        //return 0;
        default:
            printf("Invalid choice...\n");
        }

        printf("Press Enter to Continue..");
        while ( getchar() != '\n' );
    }
}