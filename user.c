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

void intSigHandler(int sigNumber){
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


