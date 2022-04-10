#include <stdio.h>
/* librarias para os processos */
#include <sys/types.h> /*pid_t */
#include <unistd.h> /*fork and pipe*/
/*librarias do wait */
#include <sys/types.h>
#include <sys/wait.h>
/*libraria strlen*/
#include <string.h>
#include <stdlib.h>

int somar(int *array, int inicio, int fim);

void fill(int *array);


int main(void){

  int vec1[1000];
  
  int vec2[1000];

  fill(vec1);
  
  fill(vec2);

  int resultado[5];

  int readNum;

  int somafinal = 0;

  int fd[2];

  pid_t pid;
  
  int i;

  if(pipe(fd) == -1){
	  
    perror("Pipe failed");
    
    return 1;
  }

  for (i = 0; i < 5; i++) {
	  
      pid = fork();

       if(pid == 0) {
		   
        close(fd[0]);
        
        int fim = (i+1)*200;
        
        int inicio = i*200;

        int sumVec1 = somar(vec1, inicio ,fim);
        
        int sumVec2 = somar(vec2, inicio ,fim);

        int tmp = sumVec1 + sumVec2;

        write(fd[1], &tmp, sizeof(tmp));

        close(fd[1]);
        
        exit(1);
      }
  }

    close(fd[1]);
    
    int pos = 0;

    while(read(fd[0], &readNum, sizeof(readNum)) > 0){
		
      *(resultado+pos) = readNum;
      
      pos++;
      
    }

    while ((pid = wait(NULL)) >= 0);
    
    close(fd[0]);

  for (int x = 0; x < 5; x++) {
	  
    somafinal += resultado[x];
    
    printf("\nSoma dos filhos %d --> %d\n", x+1,resultado[x]);
  }
  
  printf("\n\nSoma final --> %d", somafinal);

  return 0;
}



int somar(int *array, int inicio, int fim){
	
  int sum = 0;

  for(int i = inicio; i < fim; i++){
	  
    sum += array[i];
    
  }

  return sum;
}


void fill(int *array){
	
  for(int i = 0; i < 1000; i++){
	  
    array[i] = (int) (rand() % (1001));
    
  }
}
