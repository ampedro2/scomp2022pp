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

#define NUM_ELEMENTS 1000
#define NUM_ANALYZED 100

typedef struct {
	int num[10];
} maximos;

void inicializeArray(int *array){
	srand ( time(NULL) );
	for (int i = 0; i < NUM_ELEMENTS; i++){ /* atribuir valor ao vetor */
		array[i] = rand() % 1001;
		printf("%d - %d\n", i+1, array[i]);
	}
}

int main(int argc, char *argv[]) {
	int fd, data_size = sizeof(maximos), r, i, j, status, array[NUM_ELEMENTS], maior = 0;
	maximos *shared_data;
	pid_t procid;
	inicializeArray(array);
	
	fd = shm_open("/ex9", O_CREAT|O_EXCL|O_RDWR, S_IRUSR|S_IWUSR);
	if(fd == -1){
		printf("Erro a abrir");
		exit(0);
	}
	ftruncate(fd, data_size);
	shared_data = (maximos *)mmap(NULL, data_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

	for (i = 0; i < NUM_ELEMENTS/NUM_ANALYZED; i++){
		procid = fork();
		if (procid > 0) {
			waitpid(procid, &status, 0);	
		} 
		else {
			for(j = i*NUM_ANALYZED; j < (i+1)*NUM_ANALYZED; j++){
				if (array[j] > maior){
					maior = array[j];
				}	
			}
			shared_data->num[i] = maior;
			printf("Maximo calculado no intervalo[%d-%d]: %d\n", i*NUM_ANALYZED, (i+1)*NUM_ANALYZED, maior);
			exit(1);
		}
	}
	
	for (i = 0; i < 10; i++){
		if (shared_data->num[i] > maior){
			maior = shared_data->num[i];
		}
	}
	
	printf("Valor maximo calculado: %d\n", maior);
	
	r = munmap(shared_data, data_size);
	if (r < 0) exit(1);
	r = close(fd);
	if (r < 0) exit(1);
	r = shm_unlink("/ex9");
	if (r < 0) exit(1);
	exit(0);
}
