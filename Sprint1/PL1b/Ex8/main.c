#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

int qtd = 5;

int sigaction(int sig,
const struct sigaction * act,
struct sigaction * oact);

void handle_USR1(int signo, siginfo_t *sinfo, void *context) {
	
  qtd--;
  
  char *str = "Filho terminou\n";
  
  write(STDOUT_FILENO,str,strlen(str));
}

void print(int max, int min, int processo);

int main() {

  struct sigaction act;

  memset(&act, 0, sizeof(struct sigaction));

  sigfillset(&act.sa_mask);

  act.sa_sigaction = handle_USR1;
  
  act.sa_flags = SA_SIGINFO;
  
  sigaction(SIGUSR1, &act, NULL);

  int p;
  
  int filhos[5];

  for (int i = 0; i < 5; i++) {
	  
    p = fork();
    
    if (p == 0) {
		
      int PIDparente = getppid();
      
      print((i+5)*200,i*200,i+1);
      
      kill(PIDparente,10);
      
      exit(i+1);
      
    } else {
		
      filhos[i] = p;
      
    }
  }

  while(qtd!=0){
	  
    pause();
    
  }

  wait(filhos);

}

void print(int max, int min, int processo){
	
  int maxx = max;
  
  int minn = min;

  for(int i = minn; i <= maxx; i++){
	  
      printf("Processo %d = %d\n",processo,i);
      
  }
}
