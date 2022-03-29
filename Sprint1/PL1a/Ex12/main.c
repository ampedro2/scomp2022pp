#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

int n = 6;

int spawn_childs(int n){

  pid_t pp;
  
  for (int i = 0; i < n; i++) {
  
    pp = fork();
    
    if (pp == 0) {
      return (i+1)*2;
    }
    
  }

  while ((pp = wait(NULL)) >= 0);
  
  return 0;
}


int main(void){

  int p =  spawn_childs(n);

  if (p > 0) {
      printf("Filho = %d ; IDx2 = %d\n",p/2,p);
      exit(p);
  } 
  
  else {
      printf("ID do Pai = %d\n",p);
  }
  
}

