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
	int maxIndex;
	int stop;
	int canWrite;
	int canRead;
	int lastValue;
} circularBuffer;

int main(int argc, char *argv[]) {
	
	shm_unlink("/ex14");
	int fd;
	int data_size = sizeof(circularBuffer);
	circularBuffer *shared_data;
	fd = shm_open("/ex14", O_CREAT|O_EXCL|O_RDWR,S_IRUSR|S_IWUSR);
	ftruncate (fd, data_size);
	shared_data = (circularBuffer *)mmap(NULL,data_size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);


	memset( shared_data->array,0, sizeof( shared_data->array));
	shared_data->canRead = 0;
	shared_data->canWrite = 1;
	shared_data->maxIndex = 9;
	shared_data->realIndex = 0;
	shared_data->stop = 0;
	shared_data->lastValue = 0;


	int pid = fork();

	if (pid == -1) {
	   perror("Erro fork");
	   exit(EXIT_FAILURE);
	}

	if (pid > 0) {
		int ronda = 0;
		while(ronda < 3){
			printf("Ronda %d\n", ronda+1);
			do {
				while(shared_data->canWrite == 0){}
				printf("Inserir inteiro %d: ",shared_data->realIndex);
				scanf("%d", &shared_data->array[shared_data->realIndex]);
				shared_data->canWrite = 0;
				shared_data->canRead = 1;
			} while(shared_data->realIndex < shared_data->maxIndex);
			while(shared_data->canWrite == 0){}
			shared_data->realIndex=0;
			shared_data->lastValue = shared_data->array[9];
			ronda++;
		}
		shared_data->canRead = 1;
		shared_data->stop = 1;

		waitpid(pid, NULL, 0);

	} else {
		while(shared_data->stop == 0){

			while(shared_data->canRead == 0){}
			
			if (shared_data->stop == 1){
				exit (0);
			} else if (shared_data->realIndex != 0 && shared_data->array[shared_data->realIndex] <= shared_data->array[shared_data->realIndex-1] ) {
				printf("O valor recebido foi %d, mas o anterior foi %d\n", shared_data->array[shared_data->realIndex], shared_data->array[shared_data->realIndex-1]);
				fflush(stdout);
			} else if (shared_data->realIndex == 0 && shared_data->array[shared_data->realIndex] <= shared_data->lastValue) {
                  printf("O valor recebido foi %d, mas o anterior foi %d\n", shared_data->array[shared_data->realIndex], shared_data->lastValue);
                  fflush(stdout);
			} else if (shared_data->realIndex == 0 && shared_data->array[shared_data->maxIndex] == 0) {
				printf("Valor recebido = %d\n", shared_data->array[shared_data->realIndex]);
				fflush(stdout);
				shared_data->realIndex++;
			} else {
				printf("Valor recebido = %d\n", shared_data->array[shared_data->realIndex]);
				fflush(stdout);
				shared_data->realIndex++;
			}
			shared_data->canWrite = 1;
			shared_data->canRead = 0;
		}
		exit(0);
	}
	printf("Fim\n");

	fd = munmap(shared_data, data_size);

	if (fd < 0) exit(1);

	fd = shm_unlink("/ex14");

	if (fd < 0) exit(1);
	exit(0);
}
