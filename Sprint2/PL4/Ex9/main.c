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

#define NUM_PROCESSES 9

typedef struct {
	int num_process_bar;
} estrutura;

void buy_chips(){
	printf("The process %d bought chips.\n", getpid());
}

void buy_beer(){
	printf("The process %d bought beer.\n", getpid());
}

void eat_and_drink(){
	printf("The process %d ate and drank.\n", getpid());
}

int main(int argc, char *argv[]) {
	int fd, data_size = sizeof(estrutura), i, status, num;
	estrutura *shared_data;
	pid_t procid;
	
	shm_unlink("/Ex9shm");
	fd = shm_open("/Ex9shm", O_CREAT|O_EXCL|O_RDWR, S_IRUSR|S_IWUSR);
	if(fd == -1){
		printf("Error in shared memmory");
		exit(0);
	}
	ftruncate(fd, data_size);
	shared_data = (estrutura *)mmap(NULL, data_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	
	shared_data->num_process_bar = 0;
	
	sem_t *sem_nproc = sem_open("Ex9nproc", O_CREAT|O_EXCL,0644, 1); //semáforo que controla o incremento de processos retidos na barreira
	
	if(sem_nproc == -1){
		perror("Error in semaphore creation.\n");
		exit(0);
	}

	sem_t *sem_barrier = sem_open("Ex9barrier", O_CREAT|O_EXCL, 0644, 0); //barreira que irá bloquear os processos
	
	if(sem_barrier == -1){
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
			num = rand() % 10 + 1;
			printf("The process %d is waiting %d second(s).\n", getpid(), num);
			sleep(num); //dorme um tempo random
			
			num = rand() % 2 + 1; // de modo definir se o processo compra chips ou beer, é gerado um número e consoante o seu valor, o processo faz uma ou outra atividade
			if (num == 1){
				buy_chips();
			}
			else{
				buy_beer();
			}
			
			sem_wait(sem_nproc); // Não permitir que outros vários processos atualizem o valor ao mesmo tempo
			shared_data->num_process_bar++;
			sem_post(sem_nproc); // Permite a continuidade aos outros processos
			
			if (shared_data->num_process_bar == NUM_PROCESSES) sem_post(sem_barrier); // caso todos os processos estão retidos na barreira, esta é levantada
			
			sem_wait(sem_barrier); // Enquanto o processo fica à espera que os outros executem as suas atividades, este fica retido na barreira 
			eat_and_drink();
			sem_post(sem_barrier); // Permite que outros processos se libertem da barreira
			exit(1);
		}
	}
	
	for(i = 0; i < NUM_PROCESSES; i++){
		wait(&status);
	}
	
	if (sem_close(sem_nproc) == -1){
		perror("Error closing the semaphore\n");
		exit(0);
	}

	if (sem_unlink("Ex9nproc") == -1){
		perror("Error removing the semaphore.\n");
		exit(0);
	}
	
	if (sem_close(sem_barrier) == -1){
		perror("Error closing the semaphore\n");
		exit(0);
	}

	if (sem_unlink("Ex9barrier") == -1){
		perror("Error removing the semaphore.\n");
		exit(0);
	}
	
	return 0;
}
