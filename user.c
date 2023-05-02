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
int index;
int *shmChild;
int *shmTerminate;
char errmsg[200];
struct timer *shmTime;
struct resources *shmResources;
pid_t pid;
sem_t *semDead;
sem_t *semTerminate;
sem_t *semChild;
