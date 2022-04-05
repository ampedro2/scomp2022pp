#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

int usr1_qtd=0;

void handle_signal(int signo, siginfo_t *sinfo, void *context)
{
	usr1_qtd++;

	write(STDOUT_FILENO, "SIGUSR1 signal captured, USR1_COUNTER = %d\n",usr1_qtd,42);
}



int main() {
	
	struct sigaction act;
	
	memset(&act, 0, sizeof(struct sigaction));
	
    sigemptyset(&act.sa_mask); 
    
    act.sa_sigaction = handle_signal;
    
    act.sa_flags = SA_SIGINFO;
    
    sigaction(SIGUSR1, &act, NULL);
    
	for(;;){
		
		printf("signal\n");
		
	    sleep(1);
 
	}
	

	
}
