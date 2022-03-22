#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

int main() {
  int i, par = 0;
  pid_t p;
  int lista[4];
  /*lista = (int*) malloc(4*sizeof(int));*/
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
        lista[par] = (int) p;
        par++;
      }
    } else {
      printf("Child.\n");
      sleep(1);
      exit(1);
    }
  }
  if (p != 0) {
    printf("Wait\n");
    wait(lista);
  }

  d)

*/

for (i = 0; i < 4; i++) {
  p = fork();
  if (p != 0) {
    if( ((int) p ) %2 ==0){
      printf("Pid do processo filho par ==> c%d\n",(int) p);
      lista[par] = (int) p;
      par++;
    }
  } else {
    exit(i);
  }
}

if (p != 0) {
  printf("Wait\n");
  wait(lista);
}
}
