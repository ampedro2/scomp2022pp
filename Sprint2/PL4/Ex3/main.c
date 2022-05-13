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

#define NUM_PROCESS 10

typedef struct {
	int index;
    char strings[50][80];
} estrutura;

int main(int argc, char *argv[]) {
	int fd, data_size = sizeof(estrutura), status, i, num, r;
	estrutura *shared_data;
	pid_t procid;
	
	shm_unlink("/Ex3shm");
	fd = shm_open("/Ex3shm", O_CREAT|O_EXCL|O_RDWR, S_IRUSR|S_IWUSR);
	if(fd == -1){
		printf("Error in shared memmory");
		exit(0);
	}
	ftruncate(fd, data_size);
	shared_data = (estrutura *)mmap(NULL, data_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	
	shared_data->index = 0;
    
    sem_unlink("/Ex3sem");
	sem_t *sem = sem_open("/Ex3sem", O_CREAT|O_EXCL, 0644, 1);
	
	if(sem == -1){
		perror("Error in semaphore creation\n");
		exit(0);
	}
	
	for (i = 0; i < NUM_PROCESS; i++){
		procid = fork();
		if (procid == 0){
			sem_wait(sem); // não permite a que outros processos procedam ao acesso à memória partilhada
			if (shared_data->index < NUM_PROCESS){
				sprintf(shared_data->strings[shared_data->index], "I'm the Father - with PID %d", getpid()); //escreve na memória partilhada a mensagem juntamente com o pid do processo
				shared_data->index++; //Permite o processo saber onde escrever cada mensagem
			}
			
			srand(time( NULL ));
			num = rand() % 4 + 1;
			printf("The process %d is waiting %d second(s).\n", getpid(), num);
			sleep(num); 
			sem_post(sem); // Desbloqueia o acesso dos outros processos à memória partilhada
			exit(1);
		}
	}

	for(i = 0; i < NUM_PROCESS; i++){
		wait(&status);
	}
	
	for (i = 0; i < NUM_PROCESS; i++){
		printf("%d - %s\n", i+1, shared_data->strings[i]);
	}
	
	r = munmap(shared_data, data_size);
	if (r < 0) exit(1);
	r = close(fd);
	if (r < 0) exit(1);
	r = shm_unlink("/Ex3shm");
	if (r < 0) exit(1);
	exit(0);
	
	if (sem_close(sem) == -1){
        perror("Error closing the semaphore\n");
        exit(0);
    }

    if (sem_unlink("/Ex3sem") == -1){
        perror("Error removing the semaphore.\n");
        exit(0);
    }
	
	return 0;
}
