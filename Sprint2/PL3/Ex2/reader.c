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
  int productCode;
  char description[30];
  float price;
  int canRead;
  int canWrite;
} product;

int main(int argc, char *argv[]) {

    int fd, data_size = sizeof(product);

    product *shared_data;

    fd = shm_open("/ex2", O_EXCL|O_RDWR,S_IRUSR|S_IWUSR);
    
    if (fd == -1) {
        printf("Erro a abrir");
        exit(1);
    }

    ftruncate (fd, data_size);

    shared_data = (product *)mmap(NULL,data_size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);

    while(shared_data->canRead == 0){}
      printf("\nProduct code: %d",shared_data->productCode);
      printf("\nProduct description: %s",shared_data->description);
      printf("Product price: %.2f€\n",shared_data->price);

    fd = munmap(shared_data, data_size);
    if (fd < 0) exit(1);

    fd = shm_unlink("/ex2");
    if (fd < 0) exit(1);
    exit(0);
}
