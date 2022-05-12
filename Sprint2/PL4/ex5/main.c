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


int main(int argc, char *argv[]) {
	pid_t procid;
    
	sem_t *sem = sem_open("/Ex5", O_CREAT|O_EXCL, 0644, 0); //criação de um semáforo com o valor inicial de 0
	if(sem == -1){
		perror("Error in semaphore creation\n");
		exit(0);
	}
	
	procid = fork();
	if(procid == -1){
		perror("Error in process creation\n");
		exit(0);
	}
	else if (procid == 0){
		printf("I'm the child\n");
		sem_post(sem); //Só depois de escrever a mensagem, permite que o pai escreva a sua 
		exit(1);
	}
	else{
		sem_wait(sem); //Espera o filho escrever a mensagem e incrementar o valor do semáforo para continuar 
		printf("I'm the father\n");
		sem_post(sem);
	}
	
	if (sem_close(sem) == -1){
        perror("Error closing the semaphore\n");
        exit(0);
    }

    if (sem_unlink("/Ex5") == -1){
        perror("Error removing the semaphore.\n");
        exit(0);
    }
	
	return 0;
}
