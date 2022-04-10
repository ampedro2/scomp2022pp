#include <stdio.h>
#include <sys/types.h> 
#include <unistd.h> 
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>

int main(void){

  int integer = 0;

  char read_msg[80];

  char write_msg[80];

  int fd[2];

  pid_t pid;

  if(pipe(fd) == -1){
    perror("Pipe failed");
    return 1;
  }

  pid = fork();
  
  if(pid > 0){

    close(fd[0]);

    scanf("%d", &integer);

    write(fd[1], &integer, sizeof(integer));
    
    scanf("%s", &write_msg);

    write(fd[1], write_msg, sizeof(write_msg));

    close(fd[1]);

  } else {

    close(fd[1]);

    read(fd[0], &integer, sizeof(integer));
    printf("\ninteger --> %d", integer);

    read(fd[0], read_msg, sizeof(read_msg));
    printf("\ncharacter --> %s", read_msg);

    close(fd[0]);
    exit(1);
  }

  return 0;
}
