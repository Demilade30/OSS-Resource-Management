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
#define outputMax 100000

struct timer{
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
int shmIDTime;
int shmIDChild;
int shmIDTerminate;
int shmIDResources;
int lockProcess[18] = {0};
int logCount = 0;
int totalLocked = 0;
char errmsg[200];
struct timer *shmTime;
struct resources *shmResources;
pid_t pid;
sem_t *semDead;
sem_t *semTerminate;
sem_t *semChild;
FILE *fp;

void intSignalHandler(int sigNumber){
	//send message to stderr
	snprintf(errmsg, sizeof(errmsg), "oss: caught SIGINT, killing all child processes.");
	perror(errmsg);

	//deallocate shared memory
	errno = shmdt(shmTime);
	if(errno == -1){
		snprintf(errmsg, sizeof(errmsg), "oss: shmdt(shmTime)");
		perror(errmsg);	
}
	errno = shmctl(shmIDTime, IPC_RMID, NULL);
	if(errno == -1){
		snprintf(errmsg, sizeof(errmsg), "oss: shmctl(shmIDTime)");
		perror(errmsg);
}	
	errno = shmdt(shmChild);
	if(errno == -1){
		snprintf(errmsg, sizeof(errmsg), "oss: shmdt(shmChild)");
		perror(errmsg);	
}

	
}
