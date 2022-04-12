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

typedef struct {
	int numero;
	char nome[20];
} aluno;

int main(int argc, char *argv[]) {
	int fd, data_size = sizeof(aluno), numeroLer, r;
	aluno *shared_data;
	char nomeLer[20];
	
	fd = shm_open("/ex1", O_CREAT|O_EXCL|O_RDWR,S_IRUSR|S_IWUSR);
	if(fd == -1){
		printf("Erro a abrir");
		exit(0);
	}
	ftruncate (fd, data_size);
	shared_data = (aluno *)mmap(NULL,data_size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
	printf("Nome: ");
	scanf("%s", nomeLer);
	printf("Numero: ");
	scanf("%d", &numeroLer);
	strcpy(shared_data->nome,nomeLer);
	shared_data->numero = numeroLer;
	
	r = munmap(addr, data_size);
	if (r < 0) exit(1);
	r = close(fd);
	if (r < 0) exit(1);
	exit(0);
}

