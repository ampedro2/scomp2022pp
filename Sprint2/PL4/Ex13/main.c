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

#define NUM_PROCESSES 2
#define EXCHANGE_DATA 30

typedef struct {
	int numbers[10];
	int head;
	int tail;
	int value;
} data;

int main(int argc, char *argv[]) {
	int fd, data_size = sizeof(data), i, j, status, num, r;
	data *shared_data;
	pid_t procid;
	
	shm_unlink("/Ex13shm");
	fd = shm_open("/Ex13shm", O_CREAT|O_EXCL|O_RDWR, S_IRUSR|S_IWUSR);
	if(fd == -1){
		printf("Error in shared memmory");
		exit(0);
	}
	ftruncate(fd, data_size);
	shared_data = (data *)mmap(NULL, data_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	
	shared_data->head = 0;
	shared_data->tail = 0;
	shared_data->value = 0;
	
	sem_t *sem_excl = sem_open("Ex13excl", O_CREAT|O_EXCL,0644, 1); // semaforo que mantem a mutual exclusion
	sem_t *sem_items = sem_open("Ex13items", O_CREAT|O_EXCL,0644, 0); // semaforo que bloqueia o consumer caso não haja valores a ser lidos
	sem_t *sem_espaco = sem_open("Ex13espaco", O_CREAT|O_EXCL,0644, 10); // semaforo que bloqueia os producers caso os valores não estejam a ser lidos atempadamente
	
	if(sem_excl == -1){
		perror("Error in semaphore creation.\n");
		exit(0);
	}
	
	if(sem_items == -1){
		perror("Error in semaphore creation.\n");
		exit(0);
	}
	
	if(sem_espaco == -1){
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
			for (j = 0; j < EXCHANGE_DATA/2; j++){
				sem_wait(sem_espaco); // verifica se existe espaco para escrever, caso não haja bloqueia
				sem_wait(sem_excl); // verifica que mais nenhum processo está a ler ou escrever da memória partilhada (mutual exclusion)
				
				shared_data->numbers[shared_data->head] = shared_data->value;
				
				if (shared_data->head == 9) shared_data->head = -1; // caso o producer possa escrever valores, mas o buffer está cheio, ele excreve a partir da posição inicial do buffer

				shared_data->head++; // guarda a posição onde posteriormente irá ser escrito o próximo valor
				shared_data->value++; // incrementa o valor a ser introduzido
				
				sem_post(sem_excl); // desbloqueia outros processos que possam estar à espera para ler/escrever
				sem_post(sem_items); // aumenta o valor do semaforo informando o consumidor que há mais um valor a ler
			}
			exit(1);
		}
	}
	
	for (i = 0; i < EXCHANGE_DATA; i++){
		sem_wait(sem_items); // bloqueia caso não haja nenhum valor a ler
		sem_wait(sem_excl); // verifica que mais nenhum processo está a ler ou escrever da memória partilhada (mutual exclusion)

		num = shared_data->numbers[shared_data->tail];

		printf("The value %d was read in %d index.\n", num, shared_data->tail);

		if (shared_data->tail == 9) shared_data->tail = -1;

		shared_data->tail++;
		
		sem_post(sem_excl); // desbloqueia outros processos que possam estar à espera para ler/escrever
		sem_post(sem_espaco); // aumenta o valor do semaforo informando os produtores que há mais espaco para escrever um valor
	}
	
	for(i = 0; i < NUM_PROCESSES; i++){
		wait(&status);
	}
	
	if (sem_close(sem_excl) == -1){
		perror("Error closing the semaphore\n");
		exit(0);
	}

	if (sem_unlink("Ex13excl") == -1){
		perror("Error removing the semaphore.\n");
		exit(0);
	}
	
	if (sem_close(sem_items) == -1){
		perror("Error closing the semaphore\n");
		exit(0);
	}

	if (sem_unlink("Ex13items") == -1){
		perror("Error removing the semaphore.\n");
		exit(0);
	}
	
	if (sem_close(sem_espaco) == -1){
		perror("Error closing the semaphore\n");
		exit(0);
	}

	if (sem_unlink("Ex13espaco") == -1){
		perror("Error removing the semaphore.\n");
		exit(0);
	}
	
	r = munmap(shared_data, data_size);
	if (r < 0) exit(1);
	r = close(fd);
	if (r < 0) exit(1);
	r = shm_unlink("/Ex13shm");
	if (r < 0) exit(1);
	exit(0);
	
	return 0;
}
