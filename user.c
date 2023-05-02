#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <fcntl.h>
#define margin 10
#define bounds 500

struct timer {
	unsigned int seconds;
	unsigned int nanoseconds;
};

struct resources {
	unsigned int maxAmount;
	unsigned int available;
	unsigned int request;
	unsigned int allocation;
	unsigned int release;
	unsigned int requestArray[18];
	unsigned int allocationArray[18];
	unsigned int releaseArray[18];
	int shared;
};

int errno;
int position;
int *shmChild;
int *shmTerminate;
char errmsg[200];
struct timer *shmTime;
struct resources *shmResources;
pid_t pid;
sem_t *semDead;
sem_t *semTerminate;
sem_t *semChild;

void intSignalHandler(int sigNumber){
	int i;
	snprintf(errmsg, sizeof(errmsg), "user %d: caught sigint, killing process #%d", pid, position);
	perror(errmsg);

	for(i = 0; i < 20; i++){
	shmResources[i].releaseArray[position] = shmResources[i].allocationArray[position];
	shmResources[i].requestArray[position] = 0;
}

	sem_wait(semTerminate);
	shmTerminate[position] = 1;
	shmTerminate[19]++;
	sem_post(semTerminate);
	sem_post(semDead);

	errno = shmdt(shmTime);
	if(errno == -1){
		snprintf(errmsg, sizeof(errmsg), "user %d: shmdt(shmTime)", pid);
		perror(errmsg);
}
	errno = shmdt(shmChild);
	if(errno == -1){
		snprintf(errmsg, sizeof(errmsg), "user %d: shmdt(shmChild)", pid);
		perror(errmsg);
} 
	errno = shmdt(shmTerminate);
	if(errno == -1){
		snprintf(errmsg, sizeof(errmsg), "user %d: shmdt(shmTerm)", pid);
		perror(errmsg);
}
	errno = shmdt(shmResources);
	if(errno == -1){
		snprintf(errmsg, sizeof(errmsg), "user %d: shmdt(shmResources)", pid);
		perror(errmsg);
}
	exit(sigNumber);
}

int main(int argc, char *argv[]){
	int i;
	int j;
	int terminate = 0;
	int timeKey = atoi(argv[1]);
	int childKey = atoi(argv[2]);
	int index = atoi(argv[3]);
	int terminateKey = atoi(argv[4]);
	int resourceKey = atoi(argv[5]);
	unsigned int nextResource;
	struct timer terminateTime;
	struct timer initTime;
	signal(SIGINT, intSignalHandler);
	pid = getpid();
	key_t keyTime = 8675;
	key_t keyChild = 5309;
	key_t keyTerminate = 1138;
	key_t keyResource = 8311;
	position = index;

	//random number generator	
	srand(pid *time(NULL));

	//attach memory
	//point shmTime to shared memory
	shmTime = shmat(timeKey, NULL, 0);
	if((void *)shmTime == (void *)-1){
		snprintf(errmsg, sizeof(errmsg), "user: shmat(shmIDTime)");
		perror(errmsg);
		exit(1);
}
	//point shmChild to shared memory
	shmChild = shmat(childKey, NULL, 0);
	if((void *)shmChild == (void *)-1){
		snprintf(errmsg, sizeof(errmsg), "user: shmat(shmIDChild)");
		perror(errmsg);
		exit(1);		
}
	//point shmTerminate to shared memory
	shmTerminate = shmat(terminateKey, NULL, 0);
	if((void *)shmTerminate == (void *)-1){
		snprintf(errmsg, sizeof(errmsg), "user: shmat(shmIDTerminate)");
		perror(errmsg);
		exit(1);	
}
	//point shmResources to shared memory
	shmResources = shmat(resourceKey, NULL, 0);
	if ((void *)shmRes == (void *)-1){
		snprintf(errmsg, sizeof(errmsg), "user: shmat(shmIDResources)");
		perror(errmsg);
    		exit(1);
}
	//create a semaphore
	//open semaphore
	
}
