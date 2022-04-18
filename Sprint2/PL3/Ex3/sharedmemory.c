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
	
	int fd, data_size = sizeof(estrutura)*NUM_ELEMENTS , r, i, status;
	estrutura array[NUM_ELEMENTS], *shared_data;
	pid_t procid;
	double inicio, final;
	inicializeArray(array);
	shm_unlink("/ex3");
	fd = shm_open("/ex3", O_CREAT|O_EXCL|O_RDWR, S_IRUSR|S_IWUSR);
	if(fd == -1){
		printf("Erro a abrir");
		exit(0);
	}
	ftruncate(fd, data_size);
	shared_data = (estrutura *)mmap(NULL, data_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	
	procid = fork();
	shared_data->write_flag = 0;
	inicio = clock();
	
	if (procid > 0) {
		for(i = 0; i < NUM_ELEMENTS; i++){
			while(shared_data->write_flag);
			shared_data = &array[i];
			shared_data->write_flag = 1;
		}		
	} 
	else {
		estrutura array2[NUM_ELEMENTS];
		for(i = 0; i < NUM_ELEMENTS; i++){
			while(!shared_data->write_flag);
			array2[i] = *(shared_data);
			shared_data->write_flag = 0;	
		}
		exit(1);
	}
	
	wait(&status);
	
	final = clock();
	
	printf("O tempo de transferencia do array entre os dois processos por shared memory foi de: %fs", (final-inicio)/CLOCKS_PER_SEC);
	
	r = munmap(shared_data, data_size);
	if (r < 0) exit(1);
	r = close(fd);
	if (r < 0) exit(1);
	r = shm_unlink("/ex3");
	if (r < 0) exit(1);
	exit(0);
}
