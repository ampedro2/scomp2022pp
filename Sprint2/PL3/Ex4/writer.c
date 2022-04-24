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
  char array[100];
  int canRead;
  int canWrite;
} struct_array;


void randomChar(char *array){
  int i;
  int upper = 90;
    for (i = 0; i < 100; i++) {
      array[i] = (int) (rand() % (90 - 65 + 1)) + 65;
    }
}

int main(int argc, char *argv[]) {

      int fd, data_size = sizeof(struct_array);

      struct_array *shared_data;

      fd = shm_open("/ex4", O_CREAT|O_EXCL|O_RDWR,S_IRUSR|S_IWUSR);

      if (fd == -1) {
          printf("Erro\n");
          exit(1);
      }

      ftruncate(fd, data_size);

      shared_data = (struct_array*)mmap(NULL,data_size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);

      shared_data->canRead = 0;
      shared_data->canWrite = 1;

      while (shared_data->canWrite == 1) {
          randomChar(shared_data->array);
          shared_data->canWrite = 0;
          shared_data->canRead = 1;
      }

      fd = munmap(shared_data, data_size);
      if (fd < 0) exit(1); 

      fd = close(fd);
      if (fd < 0) exit(1);
      exit(0);

}
