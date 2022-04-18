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

#define NUM_TIMES 100000

typedef struct {
	int numero1;
	int numero2;
} numeros;

int main(int argc, char *argv[]) {
	
	int fd, data_size = sizeof(numeros), r, i, status;
	numeros *shared_data;
	pid_t procid;
	shm_unlink("/ex5");
	fd = shm_open("/ex5", O_CREAT|O_EXCL|O_RDWR, S_IRUSR|S_IWUSR);
	if(fd == -1){
		printf("Erro a abrir");
		exit(0);
	}
	ftruncate(fd, data_size);
	shared_data = (numeros *)mmap(NULL, data_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	
	shared_data->numero1 = 8000;
	shared_data->numero2 = 200;
	procid = fork();
	
	if (procid > 0) {
		for(i = 0; i < NUM_TIMES; i++){
			shared_data->numero1++;
			shared_data->numero2--;
		}		
	} 
	else {
		for(i = 0; i < NUM_TIMES; i++){
			shared_data->numero1--;
			shared_data->numero2++;	
		}
		exit(1);
	}
	
	wait(&status);
	
	printf("Valor final dos dois numeros: %d %d", shared_data->numero1, shared_data->numero2);
	
	r = munmap(shared_data, data_size);
	if (r < 0) exit(1);
	r = close(fd);
	if (r < 0) exit(1);
	r = shm_unlink("/ex5");
	if (r < 0) exit(1);
	exit(0);
}
