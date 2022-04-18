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

#define NUM_ELEMENTS 10

typedef struct {
	int numeros[10];
} number;

int main(int argc, char *argv[]) {
	int fd, data_size = sizeof(number), r, i;
	double sum = 0;
	number *shared_data;
	
	fd = shm_open("/ex7", O_RDWR, S_IRUSR|S_IWUSR);
	if(fd == -1){
		printf("Erro a abrir no reader");
		exit(0);
	}
	ftruncate(fd, data_size);
	shared_data = (number *)mmap(NULL, data_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	
	for (i = 0; i < NUM_ELEMENTS; i++){
		sum += shared_data->numeros[i];
	}
	
	printf("A media dos numeros gerados foi de: %.2f", sum/NUM_ELEMENTS);
	
	r = munmap(shared_data, data_size);
	if (r < 0) exit(1);
	r = close(fd);
	if (r < 0) exit(1);
	r = shm_unlink("/ex7");
	if (r < 0) exit(1);
	exit(0);
}
