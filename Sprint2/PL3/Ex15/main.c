#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h> 

typedef struct{
	int array[10];
	int realIndex;
	int stop;
	int lastValue;
} circularBuffer;

int main(int argc, char *argv[]) {
	int consumer[2], producer[2], readvalue = 1, writevalue = 0;
	
	if (pipe(consumer) == -1){ /* caso a criação do pipe dê erro, é imprimida a mensagem 'Pipe up failed' e o processo acaba */
		perror("Pipe up failed\n");
		exit(0);
	}
	
	if (pipe(producer) == -1){ /* caso a criação do pipe dê erro, é imprimida a mensagem 'Pipe up failed' e o processo acaba */
		perror("Pipe up failed\n");
		exit(0);
	}
	
	shm_unlink("/ex15");
	int fd;
	int data_size = sizeof(circularBuffer);
	circularBuffer *shared_data;
	fd = shm_open("/ex15", O_CREAT|O_EXCL|O_RDWR,S_IRUSR|S_IWUSR);
	ftruncate (fd, data_size);
	shared_data = (circularBuffer *)mmap(NULL,data_size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);


	memset( shared_data->array,0, sizeof( shared_data->array));
	shared_data->realIndex = 0;
	shared_data->stop = 0;
	shared_data->lastValue = 0;


	int pid = fork();

	if (pid == -1) {
	   perror("Erro fork");
	   exit(EXIT_FAILURE);
	}

	if (pid > 0) {
		close(consumer[1]);
		close(producer[0]);
		int ronda = 0;
		while(ronda < 3){
			printf("Ronda %d\n", ronda+1);
			do {
				printf("Inserir inteiro %d: ",shared_data->realIndex);
				scanf("%d", &shared_data->array[shared_data->realIndex]);
				write(producer[1], &readvalue, sizeof(int));
				read(consumer[0], &writevalue, sizeof(int));
			} while(shared_data->realIndex <= 9);
			shared_data->realIndex=0;
			shared_data->lastValue = shared_data->array[9];
			ronda++;
		}
		shared_data->stop = 1;
		close(consumer[0]);
		close(producer[1]);
		waitpid(pid, NULL, 0);

	} else {
		close(consumer[0]);
		close(producer[1]);
		while(shared_data->stop == 0){

			read(producer[0], &readvalue, sizeof(int));
			
			if (shared_data->stop == 1){
				exit (0);
			} else if (shared_data->realIndex != 0 && shared_data->array[shared_data->realIndex] <= shared_data->array[shared_data->realIndex-1] ) {
				printf("O valor recebido foi %d, mas o anterior foi %d\n", shared_data->array[shared_data->realIndex], shared_data->array[shared_data->realIndex-1]);
				fflush(stdout);
			} else if (shared_data->realIndex == 0 && shared_data->array[shared_data->realIndex] <= shared_data->lastValue) {
                  printf("O valor recebido foi %d, mas o anterior foi %d\n", shared_data->array[shared_data->realIndex], shared_data->lastValue);
                  fflush(stdout);
			} else if (shared_data->realIndex == 0 && shared_data->array[9] == 0) {
				printf("Valor recebido = %d\n", shared_data->array[shared_data->realIndex]);
				fflush(stdout);
				shared_data->realIndex++;
			} else {
				printf("Valor recebido = %d\n", shared_data->array[shared_data->realIndex]);
				fflush(stdout);
				shared_data->realIndex++;
			}
			write(consumer[1], &writevalue, sizeof(int));
		}
		close(consumer[1]);
		close(producer[0]);
		exit(0);
	}
	printf("Fim\n");

	fd = munmap(shared_data, data_size);

	if (fd < 0) exit(1);

	fd = shm_unlink("/ex15");

	if (fd < 0) exit(1);
	exit(0);
}
