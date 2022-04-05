#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/wait.h>

#define N 12

int usr1_qtd=0;

int lista[]={1,1,2,2,1,1,2,1,2,1,2,1};


void handle_signal(int signo, siginfo_t *sinfo, void *context)
{
	usr1_qtd++;
	
	printf("SIGUSR1 signal captured, USR1_COUNTER = %d!\n",usr1_qtd);
}



int main() {
	
	struct sigaction act;
	
	memset(&act, 0, sizeof(struct sigaction));
	
	sigemptyset(&act.sa_mask); 
	
	act.sa_sigaction = handle_signal;
	
	act.sa_flags = SA_SIGINFO;
	
	sigaction(SIGUSR1, &act, NULL);
	
	pid_t p = fork();
	
	if(p==0){
		
		struct timespec time, time2;
		
		time.tv_sec = 0;
		
		time.tv_nsec=10000000;

		for(int i=0; i<N; i++){
			
			printf("signal");
			
			kill(getppid(),lista[i]);
			
			nanosleep(&time, &time2);
			
		}
		
		
		exit(0);
	}
	
	for(;;){
		
		printf("Im working\n");
		
		sleep(1);
	}

	
}
