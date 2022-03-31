#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

int main() {
  int i = 0;
  pid_t p;
  
  /*
  for (i = 0; i < 4; i++) {
    if (fork() == 0) {
      sleep(1);
    }
  }

b)

  for (i = 0; i < 4; i++) {
    p = fork();
    if (p == 0) {
      printf("Processo filho\n");
      exit(1);
    }
  }

c)

  for (i = 0; i < 4; i++) {
    p = fork();
    if (p != 0) {
      if( ((int) p ) %2 ==0){
        printf("Pid do processo filho par ==> c%d\n",(int) p);
      }
    } else {
      printf("Child.\n");
      sleep(1);
      exit(1);
    }
  }
  d)

*/

for (i = 0; i < 4; i++) {
  p = fork();
  if (p != 0) {
    if( ((int) p ) %2 ==0){
      printf("Pid do processo filho par ==> c%d\n",(int) p);
    }
  } else {
    exit(i);
  }
}
}
