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

float randomChar(char *array){
  int i, sum = 0;
    for (i = 0; i < 100; i++) {
      printf("\n%dº > %c > %d",i+1,array[i], (int) array[i]);
      sum += (int) array[i];
    }
    return (sum/100);
}

int main(int argc, char *argv[]) {

    int fd, data_size = sizeof(struct_array);

    struct_array *shared_data;

    fd = shm_open("/ex4", O_EXCL|O_RDWR,S_IRUSR|S_IWUSR);
    if (fd == -1) {
        printf("Erro");
        exit(1);
    }

    ftruncate (fd, data_size);

    shared_data = (struct_array *)mmap(NULL,data_size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);

    while(shared_data->canRead == 0){}
      float average = randomChar(shared_data->array);
      printf("\nMedia = %.2f\n",average);

    fd = munmap(shared_data, data_size);
    if (fd < 0) exit(1);

    fd = shm_unlink("/ex4");
    if (fd < 0) exit(1);
    exit(0);
}
