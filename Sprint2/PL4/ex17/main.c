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
#define TYPES 3
#define MAXIMUM_CAPACITY 300

typedef struct {
	int num_client;
	int waiting_clients[3];
} club;

int main(int argc, char *argv[]) {
	int fd, data_size = sizeof(club), i, status, num, r, waited = 0;
	char type[10], name[10];
	club *shared_data;
	pid_t procid;
	
	shm_unlink("/Ex17shm");
	fd = shm_open("/Ex17shm", O_CREAT|O_EXCL|O_RDWR, S_IRUSR|S_IWUSR);
	if(fd == -1){
		printf("Error in shared memmory");
		exit(0);
	}
	ftruncate(fd, data_size);
	shared_data = (club *)mmap(NULL, data_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	
	shared_data->num_client = 0;
	shared_data->waiting_clients[0] = 0;
	shared_data->waiting_clients[1] = 0;
	shared_data->waiting_clients[2] = 0;
	
	sem_t *sem_excl = sem_open("Ex17semexcl", O_CREAT|O_EXCL,0644, 1); //semáforo que controla a mutual exclusion
	sem_t *sem_club = sem_open("Ex17semclub", O_CREAT|O_EXCL,0644, MAXIMUM_CAPACITY); //semáforo que controla o fluxo de clientes no clube
		
	if(sem_club == -1){
		perror("Error in semaphore creation.\n");
		exit(0);
	}
	
	if(sem_excl == -1){
		perror("Error in semaphore creation.\n");
		exit(0);
	}
	
	sem_t *sem[TYPES];
	
	for (i = 0; i < TYPES; i++){
		sprintf(name, "Ex17sem%d", i);
		sem[i] = sem_open(name, O_CREAT|O_EXCL,0644, 0);
		if(sem[i] == -1){
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
			waited = 0;
			srand(clock());
			num = rand() % 3;
			if (num == 0) strcpy(type, "VIP");
			else if (num == 1) strcpy(type, "Special");
			else if (num == 2) strcpy(type, "Normal");
			
			sem_wait(sem_excl);
			shared_data->waiting_clients[num]++;
			if (shared_data->num_client >= MAXIMUM_CAPACITY){
				waited = 1;
				printf("The client %s %d is waiting for someone to leave.\nClients in the club: %d.\n", type, i+1, shared_data->num_client);
				sem_post(sem_excl);
				sem_wait(sem[num]);
			}
			if (waited) sem_wait(sem_excl);
			shared_data->waiting_clients[num]--;
			shared_data->num_client++;
			printf("The client %s %d entered in the club.\nClients in the club: %d.\n", type, i+1, shared_data->num_client);
			sem_post(sem_excl);
			
			sleep(5);
			
			sem_wait(sem_excl);
			if (shared_data->waiting_clients[0] > 0){
				sem_post(sem[0]);
			}
			else if (shared_data->waiting_clients[1] > 0){
				sem_post(sem[1]);
			}
			else if (shared_data->waiting_clients[2] > 0){
				sem_post(sem[2]);
			}
			shared_data->num_client--;
			printf("The client %s %d leaved the club.\nClients in the club: %d.\n", type, i+1, shared_data->num_client);
			sem_post(sem_club);
			sem_post(sem_excl);
			exit(1);
		}
	}
	
	for(i = 0; i < NUM_PROCESSES; i++){
		wait(&status);
	}
	
	if (sem_close(sem_club) == -1){
		perror("Error closing the semaphore\n");
		exit(0);
	}

	if (sem_unlink("Ex17semclub") == -1){
		perror("Error removing the semaphore.\n");
		exit(0);
	}
	
	if (sem_close(sem_excl) == -1){
		perror("Error closing the semaphore\n");
		exit(0);
	}

	if (sem_unlink("Ex17semexcl") == -1){
		perror("Error removing the semaphore.\n");
		exit(0);
	}
	
	for (i = 0; i < TYPES; i++){
		if (sem_close(sem[i]) == -1){
			perror("Error closing the semaphore\n");
			exit(0);
		}
		
		sprintf(name, "Ex17sem%d", i);
		if (sem_unlink(name) == -1){
			perror("Error removing the semaphore.\n");
			exit(0);
		}
	}
	
	r = munmap(shared_data, data_size);
	if (r < 0) exit(1);
	r = close(fd);
	if (r < 0) exit(1);
	r = shm_unlink("/Ex17shm");
	if (r < 0) exit(1);
	exit(0);
	
	return 0;
}
