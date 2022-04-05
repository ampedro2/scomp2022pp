#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

int usr1_qtd=0;

void handle_signal(int signo, siginfo_t *sinfo, void *context)
{
	usr1_qtd++;
	
	printf("SIGUSR1 signal captured, USR1_COUNTER = %d!\n",usr1_qtd);
}



int main() {
	struct sigaction act;
	
	memset(&act, 0, sizeof(struct sigaction));
	
	sigfillset(&act.sa_mask); 
	
	sigprocmask(SIG_SETMASK,&act.sa_mask, NULL);
	
	act.sa_sigaction = handle_signal;
	
	act.sa_flags = SA_SIGINFO;
	
	sigaction(SIGUSR1, &act, NULL);
	
	
	for(;;){
		
		printf("Im working!\n");
		
		sleep(1);
	}
	
}
