#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define TNUM 10

int threadNum;				//Used to assign unique numbers for threads
int token;					//Goes round the threads
pthread_mutex_t lock;		//Controls access to shared variables

void tNumGen(int *threadId) {
	pthread_mutex_lock(&lock);
	
	*threadId = threadNum++;
	
	pthread_mutex_unlock(&lock);
}

void *thread(void *stuff) {
	pthread_t mythid = pthread_self();
	
	//Get my number
	int threadId;
	tNumGen(&threadId);
	
	//Execution logic here
	while(threadId != (token-1) ){
		pthread_mutex_lock(&lock);
		if (threadId != token)
			pthread_mutex_unlock(&lock);
		else{
			token++;
			printf("Thread %d, Token = %d\n", threadId, token);
		}
	}
	pthread_mutex_unlock(&lock);

	return NULL;
}

int main(int argc, char** argv) {
	pthread_t thid[TNUM];			//Threads related operations
	int result;						//Error identification
	int i;							//Stuff
	
	//Create mutex
	if (pthread_mutex_init(&lock, NULL) != 0)
    {
        printf("\n mutex init failed\n");
        exit(-1);
    }
	
	//Give birth to new threads
	for (i = 0; i < TNUM; i++) {
		result = pthread_create(&(thid[i]), (pthread_attr_t *)NULL, thread, NULL);
		
		if (result != 0) {
			printf ("Error on thread create, return value = %d\n", result);
			exit(-1);
		}
	}

	//Waiting for child threads
	for (i = 0; i < TNUM; i++) {
		pthread_join(thid[i], (void **)NULL);
	}
	
	pthread_mutex_destroy(&lock);
	return 0;
}
