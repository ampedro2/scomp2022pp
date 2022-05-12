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

#define NUM_PROCESSES 3

int main(int argc, char *argv[]) {
	int i, status;
	pid_t procid;
	
	char semaphores_name[3][10] = {"/Ex7a", "/Ex7b", "/Ex7c"};
    sem_t *sem_vetor[NUM_PROCESSES];
    for (i = 0; i < NUM_PROCESSES; i++){
		sem_unlink(semaphores_name[i]);
		sem_vetor[i] = sem_open(semaphores_name[i],O_CREAT|O_EXCL, 0644, 0); //criação de um semáforo com o valor inicial de 0
		if(sem_vetor[i] == -1){
			perror("Error in semaphore creation\n");
			exit(0);
		}
	}
	
	for (i = 0; i < NUM_PROCESSES; i++){
		procid = fork();
		if(procid == -1){
			perror("Error in process creation\n");
			exit(0);
		}
		else if (procid == 0){
			if (i == 0){
				printf("Sistemas ");
				fflush( stdout );
				sem_post(sem_vetor[0]); //Quando é escrita a primeira mensagem, o semáforo é desbloqueado
				sem_wait(sem_vetor[2]); // Aguarda que se escreva a terceira mensagem
				printf("a ");
				fflush( stdout );
				sem_post(sem_vetor[0]); //Quando é escrita a quarta mensagem, o semáforo é desbloqueado
				exit(0);
			}		
			else if (i == 1){
				sem_wait(sem_vetor[0]); // Aguarda que se escreva a primeira mensagem
				printf("de ");
				fflush( stdout );
				sem_post(sem_vetor[1]); //Quando é escrita a segunda mensagem, o semáforo é desbloqueado
				sem_wait(sem_vetor[0]); // Aguarda que se escreva a quarta mensagem
				printf("melhor ");
				fflush( stdout );
				sem_post(sem_vetor[1]); //Quando é escrita a quinta mensagem, o semáforo é desbloqueado
				exit(0);
			}
			else if (i == 2){
				sem_wait(sem_vetor[1]); // Aguarda que se escreva a segunda mensagem
				printf("Computadores -");
				fflush( stdout );
				sem_post(sem_vetor[2]); //Quando é escrita a segunda mensagem, o semáforo é desbloqueado para a quarta mensagem ser printada
				sem_wait(sem_vetor[1]); // Aguarda que se escreva a quinta mensagem
				printf("disciplina! ");
				fflush( stdout );
				exit(0);
			}
		}
	}
	
	for(i = 0; i < NUM_PROCESSES; i++){
		wait(&status);
	}
	
	for(i = 0; i < NUM_PROCESSES; i++){
		if (sem_close(sem_vetor[i]) == -1){
			perror("Error closing the semaphore\n");
			exit(0);
		}

		if (sem_unlink(semaphores_name[i]) == -1){
			perror("Error removing the semaphore.\n");
			exit(0);
		}
	}
	
	return 0;
}
