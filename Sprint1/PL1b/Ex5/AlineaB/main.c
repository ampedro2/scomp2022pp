#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>

void handle_CTRLC(int signo, siginfo_t *sinfo, void *context) {
	
	printf("I won’t let the process end with CTRL-C!\n");
}

int main(int argc, char *argv[]){
	struct sigaction act;
	memset(&act, 0, sizeof(struct sigaction));
	sigemptyset(&act.sa_mask); /* No signals blocked */
	act.sa_sigaction = handle_CTRLC;
	act.sa_flags = SA_SIGINFO;
	sigaction(SIGINT, &act, NULL);
	
	for(;;){
		printf("Waiting for signal\n");
		sleep(2);
    }
}
