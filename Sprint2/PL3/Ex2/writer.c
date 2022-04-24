#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h> /* For constants mode */
#include <fcntl.h> /* For constants O_* */

#define SIZE_DESCRIPTION 30

typedef struct {
  int productCode;
  char description[SIZE_DESCRIPTION];
  float price;
  int canRead;
  int canWrite;
} product;

int main(int argc, char *argv[]) {

    int fd, data_size = sizeof(product);

    product *shared_data;

    fd = shm_open("/ex2", O_CREAT|O_EXCL|O_RDWR,S_IRUSR|S_IWUSR);

    if (fd == -1) {
        printf("Erro a abrir\n");
        exit(1);
    }

    ftruncate (fd, data_size);

    shared_data = (product*)mmap(NULL,data_size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);

    shared_data->canRead = 0;
    shared_data->canWrite = 1;
    int qtd = 0;

      while (shared_data->canWrite == 1) {

          printf("Insert product code: ");
          scanf("%d", &(shared_data->productCode));

          printf("Insert product description:");
          while (fgets(shared_data->description, SIZE_DESCRIPTION, stdin) != NULL){
            if (qtd > 0) {
              break;
            }
            qtd++;
          }

          printf("Insert product price: ");
          scanf("%f", &(shared_data->price));

          shared_data->canWrite = 0;
          shared_data->canRead = 1;
      }

      fd = munmap(shared_data, data_size);
      if (fd < 0) exit(1);

      fd = close(fd);
      if (fd < 0) exit(1);
      exit(0);

}
