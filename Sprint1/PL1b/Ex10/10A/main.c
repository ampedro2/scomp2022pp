#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>

int countCharacters(char *stringKeyboard);

int sigaction(int sig,
const struct sigaction * act,
struct sigaction * oact);

void handle_ALARM(int signo) {
	
  int pid = getpid();
  
  char *str = "terminou\n";
  
  write(STDOUT_FILENO, str, strlen(str));
  
  kill(pid,SIGINT);
}

int main(void){

      struct sigaction act;
      
      memset(&act, 0, sizeof(struct sigaction));
      
      sigemptyset(&act.sa_mask);
      
      act.sa_handler = handle_ALARM;
      
      sigaction(SIGALRM, &act, NULL);

      int qtd = 0;
      
      char string[100];
      
      memset(string,0,sizeof(string));

      alarm(10);

      fgets(string,100,stdin);

      alarm(0);

      qtd = countCharacters(string);
      
      printf("%d caracteres\n", qtd);
      
      sleep(20);
      
      printf("End\n");
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
