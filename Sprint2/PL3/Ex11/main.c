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

void inicializeArray(int *array){
	srand ( time(NULL) );
	for (int i = 0; i < NUM_ELEMENTS; i++){ /* atribuir valor ao vetor */
		array[i] = rand() % 1001;
		printf("%d - %d\n", i+1, array[i]);
	}
}

int main(int argc, char *argv[]) {
	int p[2], i, j, status, array[NUM_ELEMENTS], maior = 0, maiorTotal = 0;
	pid_t procid[10];
	inicializeArray(array);
	if(pipe(p) == -1){ /* cria o pipe e verifica se houve um erro */
		perror("Pipe up failed");
		return 1;
	}

	for (i = 0; i < NUM_ELEMENTS/NUM_ANALYZED; i++){
		procid[i] = fork();
		if (procid[i] == 0){
			for(j = i*NUM_ANALYZED; j < (i+1)*NUM_ANALYZED; j++){
				if (array[j] > maior){
					maior = array[j];
				}	
			}
			close(p[0]);
			write(p[1],&maior, sizeof(maior));
			close(p[1]);
			printf("Maximo calculado no intervalo[%d-%d]: %d\n", i*NUM_ANALYZED, (i+1)*NUM_ANALYZED, maior);
			exit(1);
		}
	}
	
	close(p[1]);
	for (i = 0; i < 10; i++){
		waitpid(procid[i], &status, 0);
		read(p[0], &maior, sizeof(maior));
		if (maior > maiorTotal){
			maiorTotal = maior;
		}
	}
	close(p[0]);
	
	printf("Valor maximo calculado: %d\n", maior);
	return 0;
}
