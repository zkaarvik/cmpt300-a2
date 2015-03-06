#include "main.h"


void *fnC()
{
    int i;
    for(i=0;i<1000000;i++)
    {   
        c++;
    }   
}


pthread_mutex_t count_mutex;
pthread_spinlock_t count_spinlock;
my_spinlock_t count_my_spinlock;
my_mutex_t count_my_mutex;


void *pthreadMutexTest()
{
    int i;
    for(i=0;i<numItterations;i++)
    { 
		pthread_mutex_lock(&count_mutex);
		c++;
		pthread_mutex_unlock(&count_mutex);    
    }   
}

void *pthreadSpinlockTest()
{
	int i;
    for(i=0;i<numItterations;i++)
    { 
		pthread_spin_lock(&count_spinlock);
		c++;
		pthread_spin_unlock(&count_spinlock);    
    }   
}

void *mySpinlockTASTest()
{
	//DEBUG
	pthread_t CUR_THRD = pthread_self();


	int i;
    for(i=0;i<numItterations;i++)
    { 
		my_spinlock_lockTAS(&count_my_spinlock);
		c++;
		my_spinlock_unlock(&count_my_spinlock);    
    }   
}

void *mySpinlockTTASTest()
{
	int i;
    for(i=0;i<numItterations;i++)
    { 
		my_spinlock_lockTTAS(&count_my_spinlock);
		c++;
		my_spinlock_unlock(&count_my_spinlock);    
    }  
}

void *myMutexTASTest()
{
	int i;
    for(i=0;i<numItterations;i++)
    { 
		my_mutex_lockTAS(&count_my_mutex);
		c++;
		my_mutex_unlock(&count_my_mutex);    
    }  
}

void *myMutexTTASTest()
{
	int i;
    for(i=0;i<numItterations;i++)
    { 
		my_mutex_lockTTAS(&count_my_mutex);
		c++;
		my_mutex_unlock(&count_my_mutex);    
    }  
}

int runTest(int testID)
{

	/*You must create all data structures and threads for you experiments*/
	struct timespec start;
	struct timespec stop;
	unsigned long long result; //64 bit integer

	pthread_t *threads = (pthread_t* )malloc(sizeof(pthread_t)*numThreads);	
	int i;
	int rt;

	if (testID == 0 || testID == 1 ) /*Pthread Mutex*/
	{
		c=0;

		clock_gettime(CLOCK_MONOTONIC, &start);
		for(i=0;i<numThreads;i++)
		{
			if( rt=(pthread_create( threads+i, NULL, &pthreadMutexTest, NULL)) )
			{
				printf("Thread creation failed: %d\n", rt);
				return -1;	
			}
		
		}
		
		for(i=0;i<numThreads;i++) //Wait for all threads to finish
		{
			 pthread_join(threads[i], NULL);
		}
		clock_gettime(CLOCK_MONOTONIC, &stop);

		printf("Threaded Run Pthread (Mutex) Total Count: %d\n", c);
		result=timespecDiff(&stop,&start);
		printf("Pthread Mutex time(ms): %llu\n",result/1000000);

	}

	if(testID == 0 || testID == 2) /*Pthread Spinlock*/
	{
		c=0;

		pthread_spin_init(&count_spinlock, PTHREAD_PROCESS_SHARED);

		clock_gettime(CLOCK_MONOTONIC, &start);
		for(i=0;i<numThreads;i++)
		{
			if( rt=(pthread_create( threads+i, NULL, &pthreadSpinlockTest, NULL)) )
			{
				printf("Thread creation failed: %d\n", rt);
				return -1;	
			}
		
		}
		
		for(i=0;i<numThreads;i++) //Wait for all threads to finish
		{
			 pthread_join(threads[i], NULL);
		}
		clock_gettime(CLOCK_MONOTONIC, &stop);

		printf("Threaded Run Pthread (Spinlock) Total Count: %d\n", c);
		result=timespecDiff(&stop,&start);
		printf("Pthread Spinlock time(ms): %llu\n",result/1000000);
	}

	if(testID == 0 || testID == 3) /*MySpinlockTAS*/
	{
		c=0;
		my_spinlock_init(&count_my_spinlock);

		clock_gettime(CLOCK_MONOTONIC, &start);
		for(i=0;i<numThreads;i++)
		{
			if( rt=(pthread_create( threads+i, NULL, &mySpinlockTASTest, NULL)) )
			{
				printf("Thread creation failed: %d\n", rt);
				return -1;	
			}
		
		}
		
		for(i=0;i<numThreads;i++) //Wait for all threads to finish
		{
			 pthread_join(threads[i], NULL);
		}
		clock_gettime(CLOCK_MONOTONIC, &stop);

		printf("MySpinlockTAS Total Count: %d\n", c);
		result=timespecDiff(&stop,&start);
		printf("MySpinlockTAS time(ms): %llu\n",result/1000000);
	}

	if(testID == 0 || testID == 4) /*MySpinlockTTAS*/
	{
		c=0;
		my_spinlock_init(&count_my_spinlock);

		clock_gettime(CLOCK_MONOTONIC, &start);
		for(i=0;i<numThreads;i++)
		{
			if( rt=(pthread_create( threads+i, NULL, &mySpinlockTTASTest, NULL)) )
			{
				printf("Thread creation failed: %d\n", rt);
				return -1;	
			}
		
		}
		
		for(i=0;i<numThreads;i++) //Wait for all threads to finish
		{
			 pthread_join(threads[i], NULL);
		}
		clock_gettime(CLOCK_MONOTONIC, &stop);

		printf("MySpinlockTTAS Total Count: %d\n", c);
		result=timespecDiff(&stop,&start);
		printf("MySpinlockTTAS time(ms): %llu\n",result/1000000);
	}

	if(testID == 0 || testID == 5) /*MyMutexTAS*/
	{
		c=0;
		my_mutex_init(&count_my_mutex);

		clock_gettime(CLOCK_MONOTONIC, &start);
		for(i=0;i<numThreads;i++)
		{
			if( rt=(pthread_create( threads+i, NULL, &myMutexTASTest, NULL)) )
			{
				printf("Thread creation failed: %d\n", rt);
				return -1;	
			}
		
		}
		
		for(i=0;i<numThreads;i++) //Wait for all threads to finish
		{
			 pthread_join(threads[i], NULL);
		}
		clock_gettime(CLOCK_MONOTONIC, &stop);

		printf("MyMutexTAS Total Count: %d\n", c);
		result=timespecDiff(&stop,&start);
		printf("MyMutexTAS time(ms): %llu\n",result/1000000);
	}

	if(testID == 0 || testID == 6) /*MyMutexTTAS*/
	{
		c=0;
		my_mutex_init(&count_my_mutex);

		clock_gettime(CLOCK_MONOTONIC, &start);
		for(i=0;i<numThreads;i++)
		{
			if( rt=(pthread_create( threads+i, NULL, &myMutexTTASTest, NULL)) )
			{
				printf("Thread creation failed: %d\n", rt);
				return -1;	
			}
		
		}
		
		for(i=0;i<numThreads;i++) //Wait for all threads to finish
		{
			 pthread_join(threads[i], NULL);
		}
		clock_gettime(CLOCK_MONOTONIC, &stop);

		printf("MyMutexTTAS Total Count: %d\n", c);
		result=timespecDiff(&stop,&start);
		printf("MyMutexTTAS time(ms): %llu\n",result/1000000);
	}

	return 0;
}

int testAndSetExample()
{
	volatile long test = 0; //Test is set to 0
	printf("Test before atomic OP:%ld\n",test);
	tas(&test);
	printf("Test after atomic OP:%ld\n",test);
}

int processInput(int argc, char *argv[])
{

/*testid: 0=all, 1=pthreadMutex, 2=pthreadSpinlock, 3=mySpinLockTAS, 4=mySpinLockTTAS, 5=myMutexTAS, 6=myMutexTTAS*/
	/*You must write how to parse input from the command line here, your software should default to the values given below if no input is given*/
	
	numThreads=4;
	numItterations=1000000;
	testID=0;

	int optval;
	while((optval = getopt(argc, argv, "tid")) != -1)
	{
		switch(optval)
		{
			case 't':
				numThreads = atoi(argv[optind]);
				break;
			case 'i':
				numItterations = atoi(argv[optind]);
				break;
			case 'd':
				testID = atoi(argv[optind]);
				break;
		}
	}

	printf("threads: %d\n", numThreads);
	printf("iterations: %d\n", numItterations);
	printf("testid: %d\n", testID);
	
	return 0;
}


int main(int argc, char *argv[])
{


	printf("Usage of: %s -t #threads -i #itterations -d testid\n", argv[0]);
	printf("testid: 0=all, 1=pthreadMutex, 2=pthreadSpinlock, 3=mySpinLockTAS, 4=mySpinLockTTAS, 5=myMutexTAS, 6=myMutexTTAS\n");	
	
	//testAndSetExample(); //Uncomment this line to see how to use TAS
	
	processInput(argc,argv);
	runTest(testID); 
	return 0;

}
