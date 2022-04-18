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

#define NUM_ELEMENTS 100000

typedef struct {
	int numero;
	char string[20];
	int write_flag;
} estrutura;

void inicializeArray(estrutura *array){
	for (int i = 0; i < NUM_ELEMENTS; i++){
		array[i].numero = i;
		strcpy(array[i].string, "ISEP – SCOMP 2020");
	}
}

int main(int argc, char *argv[]) {
	int pp[2], i, size_data = sizeof(estrutura), status;
	estrutura array[NUM_ELEMENTS], elemento;
	pid_t procid;
	inicializeArray(array);
	double inicio, final;
	if (pipe(pp) == -1){ /* caso a criação do pipe dê erro, é imprimida a mensagem 'Pipe up failed' e o processo acaba */
		perror("Pipe up failed\n");
		exit(0);
	}
	inicio = clock();
	procid = fork();
	if(procid > 0){
		close(pp[0]);
		for(i=0;i<NUM_ELEMENTS;i++){
			write(pp[1],&array[i],size_data+1);
		}
		close(pp[1]);
	}
	else {
		estrutura array2[NUM_ELEMENTS];
		close(pp[1]);
		for(i = 0; i < NUM_ELEMENTS; i++){
			read(pp[0], &elemento, size_data+1);
			array2[i]=elemento;
		}
		close(pp[0]);
		exit(0);
	}
	
	wait(&status);
	
	final = clock();
	
	printf("O tempo de transferencia do array entre os dois processos por pipes foi de: %fs\n", (final-inicio)/CLOCKS_PER_SEC);
}
