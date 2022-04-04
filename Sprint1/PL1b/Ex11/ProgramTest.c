#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

void HandlerFunction(int signo, siginfo_t *sinfo, void *context){
        write(1, "The command ./ProgramTest didn’t end in its allowed time!\n", 61);
        exit(0);
}

int main(int argc, char **argv)
{
	struct sigaction act; 
	memset(&act, 0, sizeof(struct sigaction));
	sigemptyset(&act.sa_mask);
	act.sa_sigaction = HandlerFunction; /* definição da função que irá tratar o sinal ALRM caso ele seja lançado */
	sigaction(SIGALRM, &act, NULL);
    
	printf("ProgramTest\n");
	time_t t;
	srand((int)time(&t) % getpid());
	int time = rand() % 10;
	printf("%d second(s)\n", time);
	sleep(time); /* caso o valor time seja maior que o tempo estipulado anteriormente, é recebido o sinal ALRM */
	printf("Execution ended!\n");
}
