#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

static int flag;

int countCharacters(char *string);

int sigaction(int sig,
const struct sigaction * act,
struct sigaction * oact);

void handle_CHILD(int signo, siginfo_t *sinfo, void *context) {
  flag = 0;
}

int main(void){

      struct sigaction act;

      memset(&act, 0, sizeof(struct sigaction));

      sigfillset(&act.sa_mask);

      act.sa_sigaction = handle_CHILD;
      
      act.sa_flags = SA_SIGINFO;
      
      sigaction(SIGCHLD, &act, NULL);

      int qtd = 0;
      
      char string[100];
      
      memset(string,0,sizeof(string));

      int p = fork();
      
      flag = -1;

      if (p !=  0) {
		  
        sleep(10);
        
        if (flag == 0) {
			
          sleep(20);

        } else {

          kill(p,SIGINT);
          
          printf("End\n");
          
        }

      } else {

        fgets(string,100,stdin);
        
        qtd = countCharacters(string);
        
        printf("%d caracteres\n", qtd);
        
        exit(qtd);
      }

}

int countCharacters(char *string){
	
  int qtd = 0;
  
  for (int i = 0; i < strlen(string); i++) {
	  
    if (string[i] != 32 && string[i] != 10) {
		
      qtd++;
      
    }
    
  }
  return qtd;
}
