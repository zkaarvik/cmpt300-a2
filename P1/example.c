#include <stdio.h>
#include <stdint.h>
#include <time.h>

unsigned long long timespecDiff(struct timespec *timeA_p, struct timespec *timeB_p)
{
  return ((timeA_p->tv_sec * 1000000000) + timeA_p->tv_nsec) -
            ((timeB_p->tv_sec * 1000000000) + timeB_p->tv_nsec); //Calculate difference between two times, returning the difference in ns
}


int main()
{
    struct timespec start; //Declares a timespec structure
    struct timespec stop;
    unsigned long long result; //64 bit integer

    clock_gettime(CLOCK_REALTIME, &start); //Gets system-wide realtime clock value and stores in start
    sleep(1); //Sleep for 1 second
    clock_gettime(CLOCK_REALTIME, &stop); //Gets system-wide realtime clock value and stores in stop

    result=timespecDiff(&stop,&start); //Retreive the difference between stop and start times in ns

    printf("CLOCK_REALTIME Measured: %llu\n",result); //Print results

    clock_gettime(CLOCK_MONOTONIC, &start); //Gets monotonic clock value and stores in start
    sleep(1);
    clock_gettime(CLOCK_MONOTONIC, &stop); //Gets monotonic clock value and stores in stop

    result=timespecDiff(&stop,&start);

    printf("CLOCK_MONOTONIC Measured: %llu\n",result);

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start); //Gets per-process clock value and stores in start
    sleep(1);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &stop); //Gets per-process clock value and stores in stop

    result=timespecDiff(&stop,&start);

    printf("CLOCK_PROCESS_CPUTIME_ID Measured: %llu\n",result);

    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start); //Gets thread specific clock value and stores in start
    sleep(1);
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &stop); //Gets thread specific clock value and stores in stop

    result=timespecDiff(&stop,&start);

    printf("CLOCK_THREAD_CPUTIME_ID Measured: %llu\n",result);
}