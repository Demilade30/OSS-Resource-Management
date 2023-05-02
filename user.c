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
	if((void *)shmRes == (void *)-1){
		snprintf(errmsg, sizeof(errmsg), "user: shmat(shmIDResources)");
		perror(errmsg);
    		exit(1);
}
	//create a semaphore
	//open semaphore
	semDead = sem_open("semDead", 1);
	if(semDead == SEM_FAILED){
		snprintf(errmsg, sizeof(errmsg), "user %d: sem_open(semDead)", pid);
		perror(errmsg);
    		exit(1);
}
	semTerminate = sem_open("semTerminate", 1);
	if(semTerminate == SEM_FAILED){
		snprintf(errmsg, sizeof(errmsg), "user %d: sem_open(semTerminate)", pid);
		perror(errmsg);
    		exit(1);
}	
	semChild = sem_open("semChild", 1);
	if(semTerminate == SEM_FAILED){
		snprintf(errmsg, sizeof(errmsg), "user %d: sem_open(semChild)", pid);
		perror(errmsg);
    		exit(1);
}
	//calculate the first request and release times
	initTime.seconds = shmTime -> seconds;
	initTime.nanoseconds = shmTime -> nanoseconds + rand() % (bounds);
	if(initTime.nanoseconds > 1000000000){
		initTime.seconds += 1;
		initTime.nanoseconds -= 1000000000;
}
	while(!terminate){
		if(rand() % 100 <= margin){
			terminate = 1;
}
		//calculate termination time
		terminationTime.seconds = shmTime -> seconds;
		terminationTime.nanoseconds = shmTime -> nanoseconds + rand() % 250000000;
		if(terminationTime.nanoseconds > 1000000000){
			terminationTime.nanoseconds -= 1000000000;
			terminationTime.seconds += 1;
}
		if(initTime.seconds <= shmTime -> seconds){
			if(initTime.nanoseconds <= shmTime -> nanoseconds || initTime.seconds < shmTime -> seconds){
				//enter critical section
				nextResource = rand() % 20;
				if(shmResources[nextResource].allocationArray[index] == 0){
					shmResources[nextResource].requestArray[index]++;
					while(shmResources[nextResource].requestArray[index]);
}
				else{
					if(rand() % 10){
						shmResources[nextResource].requestArray[index]++;
						while(shmResources[nextResource].requestArray[index]);
}
					else{
						shmResources[nextResource].releaseArray[index] = shmResources[nextResource].allocationArray[index];
}
}
				//calculate next request/release time
				initTime.seconds = shmTime -> seconds;
				initTime.nanoseconds = shmTime -> nanoseconds + rand() % (bounds);
				if(initTime.nanoseconds > 1000000000){
					initTime.nanoseconds -= 1000000000;
					initTime.seconds += 1;
}
}
}
		//wait for system clock to pass time
		while(terminationTime.seconds > shmTime -> seconds);
		while(terminationTime.nanoseconds > shmTime -> nanoseconds);
}
	//signal release of process from running process
	sem_wait(semTerminate);
	shmTerminate[index] = 1;
	sem_post(semTerminate);
	snprintf(errmsg, sizeof(errmsg), "user %d: process terminating", pid);
	perror(errmsg);
	sem_post(semDead);

	//detach memory
	errno = shmdt(shmTime);
	if(errno == -1){
		snprintf(errmsg, sizeof(errmsg), "user: shmdt(shmTime)");
		perror(errmsg);
}
	errno = shmdt(shmChild);
	if(errno == -1){
		snprintf(errmsg, sizeof(errmsg), "user: shmdt(shmChild)");
		perror(errmsg);
}
	errno = shmdt(shmTerminate);
	if(errno == -1){
		snprintf(errmsg, sizeof(errmsg), "user: shmdt(shmTerminate)");
		perror(errmsg);
}
	errno = shmdt(shmResources);
	if(errno == -1){
		snprintf(errmsg, sizeof(errmsg), "user: shmdt(shmResources)");
		perror(errmsg);
}
	exit(0);
	return 0;
}
