#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>

typedef struct {

 char str[80];
 
 int num;
 
} estrutura;

int main(void){

  estrutura Read;
  
  estrutura Write;

  int fd[2];

  pid_t pid;

  if(pipe(fd) == -1){
    perror("Pipe failed");
    return 1;
  }

  pid = fork();

  if(pid > 0){

    close(fd[0]);

	
    scanf("%d", &Write.num);

    scanf("%s", &Write.str);

    write(fd[1], &Write , sizeof(Write));

    close(fd[1]);

  } else {

    close(fd[1]);

    read(fd[0], &Read, sizeof(Read));
    
    printf("\nNumero --> %d", Read.num);
    
    printf("\nString --> %s\n",  Read.str);

    close(fd[0]);
    
    exit(1);
  }

  return 0;
}
