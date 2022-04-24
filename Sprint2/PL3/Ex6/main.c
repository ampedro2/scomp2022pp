#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>

int array[1000];

typedef struct {
  int transfer;
  struct timeval start_t;
  struct timeval end_t;
  int canRead;
  int canWrite;
} struct_array;

void randomArray(){
  int i;
    for (i = 0; i < 1000; i++) {
      array[i] = (int) (rand() % (1001));
    }
}

int main(int argc, char *argv[]) {
	
 int fd;
 int data_size = sizeof(struct_array);
 struct_array *shared_data;
 fd = shm_open("/ex6", O_CREAT|O_EXCL|O_RDWR,S_IRUSR|S_IWUSR);
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
    randomArray();
    gettimeofday(&shared_data->start_t, NULL);
    while (cont != 1000) {
      cont++;
        while (shared_data->canWrite == 0){}
          shared_data->transfer = array[cont];
          shared_data->canWrite = 0;
          shared_data->canRead = 1;
    }
    wait(NULL);
  } else {
    int cont = 0;
    while (cont != 1000) {

        while(shared_data->canRead == 0){}
        array[cont] = shared_data->transfer;
        shared_data->canWrite = 1;
        shared_data->canRead = 0;
        cont++;
    }

    gettimeofday(&shared_data->end_t, NULL);

    long seconds = (shared_data->end_t.tv_sec - shared_data->start_t.tv_sec);
    long micros = ((seconds * 1000000) + shared_data->end_t.tv_usec) - (shared_data->start_t.tv_usec);
    printf("\nTempo: %ld segundos e %ld microsegundos\n", seconds, micros);


    fd = munmap(shared_data, data_size);

    if (fd < 0) exit(1);

    fd = shm_unlink("/ex6");
 
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

    close(fd2[0]);
    gettimeofday(&sharePipes.start_t, NULL);
    for (int i = 0; i < 1000; i++) {
        sharePipes.transfer = array[i];
        write(fd2[1], &sharePipes, sizeof(sharePipes));
    }

    close(fd2[1]);


  }else{

    close(fd2[1]);

    while(read(fd2[0], &sharePipes, sizeof(sharePipes))){

    }

    gettimeofday(&sharePipes.end_t, NULL);

    long seconds = (sharePipes.end_t.tv_sec - sharePipes.start_t.tv_sec);
    long micros = ((seconds * 1000000) + sharePipes.end_t.tv_usec) - (sharePipes.start_t.tv_usec);
    printf("\nTempo: %ld segundos e %ld microsegundos\n", seconds, micros);

    close(fd2[0]);
    exit(1);
  }

  wait(NULL);
  printf("\nFim\n");
  exit(0);
}
