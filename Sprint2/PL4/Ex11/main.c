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

#define NUM_PROCESSES 400
#define MAXIMUM_CAPACITY 200

typedef struct {
	int num_passengers_flow;
	int empty_seats;
	int num_passengers_stay;
} train;

int main(int argc, char *argv[]) {
	int fd, data_size = sizeof(train), i, status, num, r;
	train *shared_data;
	pid_t procid;
	
	shm_unlink("/Ex11shm");
	fd = shm_open("/Ex11shm", O_CREAT|O_EXCL|O_RDWR, S_IRUSR|S_IWUSR);
	if(fd == -1){
		printf("Error in shared memmory");
		exit(0);
	}
	ftruncate(fd, data_size);
	shared_data = (train *)mmap(NULL, data_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	
	shared_data->num_passengers_flow = 0;
	shared_data->empty_seats = MAXIMUM_CAPACITY;
	shared_data->num_passengers_stay = 0;
	
	sem_t *sem_flow = sem_open("Ex11sem", O_CREAT|O_EXCL,0644, 3); //semáforo que controla o fluxo de passageiros no metro
	sem_t *sem_excl = sem_open("Ex11semexcl", O_CREAT|O_EXCL,0644, 1); //semáforo que controla a mutual exclusion
	sem_t *sem_train = sem_open("Ex11semtrain", O_CREAT|O_EXCL,0644, MAXIMUM_CAPACITY); //semáforo que controla a mutual exclusion
		
	if(sem_flow == -1){
		perror("Error in semaphore creation.\n");
		exit(0);
	}
	
	if(sem_excl == -1){
		perror("Error in semaphore creation.\n");
		exit(0);
	}
	
	for (i = 0; i < NUM_PROCESSES; i++){
		procid = fork();
		if(procid == -1){
			perror("Error in process creation.\n");
			exit(0);
		}
		else if (procid == 0){
			sem_wait(sem_train);
			sem_wait(sem_flow); //espera que alguma porta esteja livre para poder sair no comboio
			sem_wait(sem_excl);
			printf("The process %d entered the train.\n", getpid());
			shared_data->empty_seats--;
			printf("Number of occupied seats: %d\n", MAXIMUM_CAPACITY-shared_data->empty_seats);
			sem_post(sem_excl);
			sem_post(sem_flow);
			
			sleep(0.02);
			
			if (shared_data->num_passengers_stay >= 50){
				sem_wait(sem_flow);
				sem_wait(sem_excl);
				sem_post(sem_train);
				printf("The process %d leaved the train.\n", getpid());
				shared_data->empty_seats++;
				printf("Number of occupied seats: %d\n", MAXIMUM_CAPACITY-shared_data->empty_seats);
				sem_post(sem_flow);
				sem_post(sem_excl);
			}
			else{
				shared_data->num_passengers_stay++;
			}
			exit(1);
		}
	}
	
	for(i = 0; i < NUM_PROCESSES+3; i++){
		wait(&status);
	}
	
	if (sem_close(sem_flow) == -1){
		perror("Error closing the semaphore\n");
		exit(0);
	}

	if (sem_unlink("Ex11sem") == -1){
		perror("Error removing the semaphore.\n");
		exit(0);
	}
	
	if (sem_close(sem_excl) == -1){
		perror("Error closing the semaphore\n");
		exit(0);
	}

	if (sem_unlink("Ex11semexcl") == -1){
		perror("Error removing the semaphore.\n");
		exit(0);
	}
	
	if (sem_close(sem_train) == -1){
		perror("Error closing the semaphore\n");
		exit(0);
	}

	if (sem_unlink("Ex11semtrain") == -1){
		perror("Error removing the semaphore.\n");
		exit(0);
	}
	
	r = munmap(shared_data, data_size);
	if (r < 0) exit(1);
	r = close(fd);
	if (r < 0) exit(1);
	r = shm_unlink("/Ex11shm");
	if (r < 0) exit(1);
	exit(0);
	
	return 0;
}
