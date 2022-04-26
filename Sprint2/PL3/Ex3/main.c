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

typedef struct {
	int num;
	char string[30];
	int canRead;
	int canWrite;
} struct_array;

int main(int argc, char *argv[]) {
	double inicio, fim;
	int fd;
	int data_size = sizeof(struct_array);
	struct_array *shared_data, estrutura[NUM_ELEMENTS];
	shm_unlink("/ex3");
	fd = shm_open("/ex3", O_CREAT|O_EXCL|O_RDWR,S_IRUSR|S_IWUSR);
	ftruncate (fd, data_size);
	shared_data = (struct_array *)mmap(NULL,data_size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
	shared_data->canRead = 0;
	shared_data->canWrite = 1;
	
	int pid = fork();

	if (pid == -1) {
		perror("Erro fork");
		exit(EXIT_FAILURE);
	}

	if (pid > 0) {
		int cont = 0;

		for (int i = 0; i < NUM_ELEMENTS; i++) {
			estrutura[i].num = (int) (rand() % (NUM_ELEMENTS+1));
			strcpy(estrutura[i].string, "ISEP – SCOMP 2020");
		}
		
		inicio = clock();
		while (cont < NUM_ELEMENTS) {
			cont++;
			while (shared_data->canWrite == 0){}
			memcpy(shared_data, &estrutura[cont], sizeof(data_size));
			shared_data->canWrite = 0;
			shared_data->canRead = 1;
		}
		waitpid(pid, NULL, 0);
		
		fim = clock();

		printf("O tempo de transferencia do array entre os dois processos por memória partilhada foi de: %fs\n", (fim-inicio)/CLOCKS_PER_SEC);
		
	} else {
		int cont = 0;
		while (cont < NUM_ELEMENTS) {
			while(shared_data->canRead == 0){}
			memcpy(&estrutura[cont], shared_data, sizeof(struct_array));
			shared_data->canWrite = 1;
			shared_data->canRead = 0;
			cont++;
		}

		fd = munmap(shared_data, data_size);
		if (fd < 0) exit(1);
		fd = shm_unlink("/ex3");
		if (fd < 0) exit(1);
		exit(0);
	}

	fd = munmap(shared_data, data_size);
	if (fd < 0) exit(1);
	fd = close(fd);
	if (fd < 0) exit(1);

	struct_array sharePipes;
	int fd2[2];

	if(pipe(fd2) == -1){
		perror("Pipe failed");
		return 1;
	}

	pid = fork();

	if(pid == -1){
		perror("Fork failed");
		exit(1);
	}

	if(pid > 0){
		inicio = clock();
		close(fd2[0]);
		for (int i = 0; i < NUM_ELEMENTS; i++) {
			sharePipes = estrutura[i];
			write(fd2[1], &sharePipes, sizeof(sharePipes));
		}
		close(fd2[1]);
		waitpid(pid, NULL, 0);
		fim = clock();

		printf("O tempo de transferencia do array entre os dois processos por pipes foi de: %fs\n", (fim-inicio)/CLOCKS_PER_SEC);
	}else{
		close(fd2[1]);
		
		while(read(fd2[0], &sharePipes, sizeof(sharePipes))){}

		close(fd2[0]);
		exit(1);
	}
	printf("\nFim\n");
	exit(0);
}
