#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <semaphore.h>
#include <wait.h>
#include <string.h>

#define NUM_PROCESSES 25
#define MAXIMUM_CAPACITY 5
#define NUM_ROOMS 4

typedef struct {
	int num_visitors[NUM_ROOMS];
	int visitors_total;
} ShowRoom;

int main(int argc, char *argv[]) {
	char name1[10], name[20];
	int fd, data_size = sizeof(ShowRoom), i, status, num, r;
	ShowRoom *shared_data;
	pid_t procid;
	
	shm_unlink("/Ex15shm");
	fd = shm_open("/Ex15shm", O_CREAT|O_EXCL|O_RDWR, S_IRUSR|S_IWUSR);
	if(fd == -1){
		printf("Error in shared memmory");
		exit(0);
	}
	ftruncate(fd, data_size);
	shared_data = (ShowRoom *)mmap(NULL, data_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	
	shared_data->num_visitors[0] = 0;
	shared_data->num_visitors[1] = 0;
	shared_data->num_visitors[2] = 0;
	shared_data->num_visitors[3] = 0;
	shared_data->visitors_total = 0;
	
	sem_t *sem[NUM_ROOMS];
	sem_t *semIsInShow[NUM_ROOMS];
	
	for (i = 0; i < NUM_ROOMS; i++){
		sprintf(name, "Ex15sem%d", i);
		sem[i] = sem_open(name, O_CREAT|O_EXCL,0644, 5);
		if(sem[i] == -1){
			perror("Error in semaphore creation.\n");
			exit(0);
		}
		
		sprintf(name, "Ex15sem%dIsFull", i);
		semIsInShow[i] = sem_open(name, O_CREAT|O_EXCL,0644, 1);
		if(semIsInShow[i] == -1){
			perror("Error in semaphore creation.\n");
			exit(0);
		}
	}
	
	for (i = 0; i < NUM_PROCESSES; i++){
		procid = fork();
		if(procid == -1){
			perror("Error in process creation.\n");
			exit(0);
		}
		else if (procid == 0){
			srand(getpid());
			num = rand() % 4;
			sem_wait(semIsInShow[num]);
			sem_post(semIsInShow[num]);
			if (shared_data->num_visitors[num] >= MAXIMUM_CAPACITY){
				printf("The room %d is full.\n", num+1);
			}
			else{
				sem_wait(sem[num]);
				printf("The process %d entered in the room %d\n", i+1, num+1);
				shared_data->num_visitors[num]++;
				shared_data->visitors_total++;
				sem_post(sem[num]);
				exit(1);
			}
		}
	}
	
	for (i = 0; i < NUM_ROOMS; i++){
		procid = fork();
		if(procid == -1){
			perror("Error in process creation.\n");
			exit(0);
		}
		else if (procid == 0){
			while ( shared_data->visitors_total <= NUM_PROCESSES ){
				sleep(1);
				sem_wait(semIsInShow[i]);
				printf("The show in the room %d is starting...\n", i+1);
				sleep(5);
				printf("The show in the room %d ended.\n", i+1);
				shared_data->num_visitors[num] = 0;
				sem_post(semIsInShow[i]);
			}
			exit(1);
		}
	}
	
	for(i = 0; i < NUM_PROCESSES+3; i++){
		wait(&status);
	}
	
	for (i = 0; i < NUM_ROOMS; i++){
		if (sem_close(sem[i]) == -1){
			perror("Error closing the semaphore\n");
			exit(0);
		}
		
		sprintf(name, "Ex15sem%d", i);
		if (sem_unlink(name) == -1){
			perror("Error removing the semaphore.\n");
			exit(0);
		}
		
		if (sem_close(semIsInShow[i]) == -1){
			perror("Error closing the semaphore\n");
			exit(0);
		}
		
		sprintf(name, "Ex15sem%dIsFull", i);
		if (sem_unlink(name) == -1){
			perror("Error removing the semaphore.\n");
			exit(0);
		}
	}
	
	r = munmap(shared_data, data_size);
	if (r < 0) exit(1);
	r = close(fd);
	if (r < 0) exit(1);
	r = shm_unlink("/Ex15shm");
	if (r < 0) exit(1);
	exit(0);
	
	return 0;
}
