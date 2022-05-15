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

#define NUM_PROCESSES 500
#define MAXIMUM_CAPACITY 200

typedef struct {
	int num_passengers_flow;
	int empty_seats;
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
	shared_data->empty_seats = 200;
	
	sem_t *sem_flow = sem_open("Ex11sem", O_CREAT|O_EXCL,0644, 0); //semáforo que controla o fluxo de passageiros no metro
	
	if(sem_flow == -1){
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
			srand(getpid());
			num = rand() % 2 + 1;
			if (num == 1){
				sem_wait(sem_flow); //espera que alguma porta esteja livre para poder sair no comboio
				if ( shared_data->empty_seats >= MAXIMUM_CAPACITY ){ //caso seja atribuida a funcao de sair do metro ao processo sendo que este está vazio, é printada uma mensagem de aviso
					printf("Train is empty!\n");
				} else{
					printf("The process %d leaved the train.\n", getpid());
					printf("Number of occupied seats: %d\n", MAXIMUM_CAPACITY-shared_data->empty_seats);
					shared_data->empty_seats++;
				}
				sem_post(sem_flow); 
				
			}
			else if (num == 2){
				sem_wait(sem_flow); //espera que alguma porta esteja livre para poder entrar no comboio
				if ( shared_data->empty_seats <= 0 ){ //caso seja atribuida a funcao de entrar do metro ao processo sendo que este está cheio, é printada uma mensagem de aviso
					printf("Train maximum capacity reached. Impossible to enter!\n");
				} else{
					printf("The process %d entered the train.\n", getpid());
					printf("Number of occupied seats: %d\n", MAXIMUM_CAPACITY-shared_data->empty_seats);
					shared_data->empty_seats--;
				}
				sem_post(sem_flow);
			}
			exit(1);
		}
	}
	
	for (i = 0; i < 3; i++){
		procid = fork();
		if(procid == -1){
			perror("Error in process creation.\n");
			exit(0);
		}
		else if (procid == 0){
			while ( shared_data->num_passengers_flow < NUM_PROCESSES ){
				sem_post(sem_flow); //desbloqueia a respetiva porta
				sleep(0.05);
				shared_data->num_passengers_flow++;
				sem_wait(sem_flow);
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
	
	r = munmap(shared_data, data_size);
	if (r < 0) exit(1);
	r = close(fd);
	if (r < 0) exit(1);
	r = shm_unlink("/Ex11shm");
	if (r < 0) exit(1);
	exit(0);
	
	return 0;
}
