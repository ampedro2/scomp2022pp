#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#define STR_SIZE 50
#define NR_DISC 10

typedef struct{
  int numero;
  char nome[STR_SIZE];
  int disciplinas[NR_DISC];
  int canRead;
}aluno;

void maior(int *disciplinas){
  int i = 0, maior = disciplinas[i];
  for (i = 1; i < NR_DISC; i++) {
      if (maior < disciplinas[i]) {
        maior = disciplinas[i];
      }
  }
  printf("\nA maior nota foi %d", maior);
}

void menor(int *disciplinas){
  int i = 0, menor = disciplinas[i];
  for (i = 1; i < NR_DISC; i++) {
      if (menor > disciplinas[i]) {
        menor = disciplinas[i];
      }
  }
  printf("\nA nota menor foi %d", menor);
}

void media(int *disciplinas){
  int i, sum = 0;
  for (i = 0; i < NR_DISC; i++) {
    sum += disciplinas[i];
  }
  printf("\nA media foi %.2f", (float) sum/NR_DISC );
}

void mostrar(aluno *shared_data){
  printf("\nNumber:%d\nNome:%s\n",shared_data->numero, shared_data->nome);
  for (int i = 0; i < NR_DISC; i++) {
    printf("Nota da disciplina %d foi %d\n",i+1,shared_data->disciplinas[i]);
  }
}

int main(int argc, char *argv[]) {

 int fd;

 int data_size = sizeof(aluno);

 aluno *shared_data;

 fd = shm_open("/ex12", O_CREAT|O_EXCL|O_RDWR,S_IRUSR|S_IWUSR);

 ftruncate (fd, data_size);
 
 shared_data = (aluno *)mmap(NULL,data_size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);

 shared_data->canRead = 0;

 int pid = fork();

  if (pid == -1) {
       perror("Erro fork");
       exit(EXIT_FAILURE);
  }
  if (pid > 0) {
        int qtd = 0;

        printf("Numero do estudante: ");
        scanf("%d", &(shared_data->numero));

        printf("Nome do estudante:");
        while (fgets(shared_data->nome, STR_SIZE, stdin) != NULL){
          if (qtd > 0) {
            break;
          }
          qtd++;
        }
        for (int i = 0; i < NR_DISC; i++) {
          printf("Classificação da disciplina %d: ",i+1);
          scanf("%d", &(shared_data->disciplinas[i]));
        }

        shared_data->canRead = 1;

    wait(NULL);

  } else {
        while(shared_data->canRead == 0){}
        shared_data->canRead = 0;
        mostrar(shared_data);
        maior(shared_data->disciplinas);
        menor(shared_data->disciplinas);
        media(shared_data->disciplinas);

    fd = munmap(shared_data, data_size);

    if (fd < 0) exit(1);

    fd = shm_unlink("/ex12");

    if (fd < 0) exit(1);
    exit(0);
  }
  printf("\nFim\n");

  fd = munmap(shared_data, data_size);

  if (fd < 0) exit(1);

  fd = close(fd);

  if (fd < 0) exit(1);
  exit(0);
}
