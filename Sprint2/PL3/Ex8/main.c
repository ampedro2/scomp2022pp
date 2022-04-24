#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

typedef struct {
  int transfer;
  int canRead;
  int canWrite;
} struct_array;

int main(int argc, char *argv[]) {
  int fd;
  int data_size = sizeof(struct_array);
  struct_array *shared_data;
  fd = shm_open("/ex8", O_CREAT|O_EXCL|O_RDWR,S_IRUSR|S_IWUSR);
  ftruncate (fd, data_size);
  shared_data = (struct_array *)mmap(NULL,data_size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);

  shared_data->transfer = 100;
  shared_data->canRead = 0;
  shared_data->canWrite = 1;


  int pid = fork();

  if (pid == -1) {
       perror("Erro fork");
       exit(EXIT_FAILURE);
  }

  if (pid > 0) {
    int cont = 0;
    while (cont != 10000) {
      cont++;
        while (shared_data->canWrite == 0){}
          shared_data->transfer++;
          printf("Número pai = %d\n", shared_data->transfer);
          shared_data->canWrite = 0;
          shared_data->canRead = 1;
    }

    wait(NULL);

  } else {
    int cont = 0;
    while (cont != 10000) {
        while(shared_data->canRead == 0){}
        shared_data->transfer--;
        printf("Número filho = %d\n", shared_data->transfer);
        shared_data->canWrite = 1;
        shared_data->canRead = 0;
        cont++;
    }

    fd = munmap(shared_data, data_size);

    if (fd < 0) exit(1);

    fd = shm_unlink("/ex8");

    if (fd < 0) exit(1);
    exit(0);
  }
  printf("Número = %d\n", shared_data->transfer);

  fd = munmap(shared_data, data_size);

  if (fd < 0) exit(1);

  fd = close(fd);

  if (fd < 0) exit(1);
  exit(0);
}
