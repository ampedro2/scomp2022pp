#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

int main(void){
  pid_t p;
  if (fork() == 0) {
    printf("PID = %d\n", getpid()); exit(0);
  }

  if ((p=fork()) == 0) {
    printf("PID = %d\n", getpid()); exit(0);
  }

  printf("PID parente = %d\n", getpid());

 printf("Waiting... (for PID=%d)\n",p);
 waitpid(p, NULL, 0);

 printf("loop\n");
 while (1);
}
