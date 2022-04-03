#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

void listBlockedSignals(int signo, siginfo_t *sinfo, void *context) {
	sigset_t cmask;
	char string[23];
	sigprocmask(0, NULL, &cmask); /* bloquear todos os sinais presentes na máscara inicializada anteriormente */
	for (int i = 1; i <= 64; i++) {
        if(sigismember(&cmask, i) == 1) { /* caso o sinal com o número i esteja na máscara completada com a função sigprocmask, é sinal que foi bloqueado */
            sprintf(string, "Signal %d - isblocked.\n", i);
            write(1, string, (sizeof(string)));
        }
        else if (i != 32 && i != 33){ /* o número 32 e 33 não correspondem a números de processos */
            sprintf(string, "Signal %d - unblocked.\n", i);
            write(1, string, (sizeof(string)));
		}
	}
		
}

int main(int argc, char *argv[]){
	
	struct sigaction act;
	memset(&act, 0, sizeof(struct sigaction));
	act.sa_sigaction = listBlockedSignals;
	sigfillset(&act.sa_mask); /* inicializa uma máscara que inclui todos os sinais */
	sigaction(SIGUSR1, &act, NULL);
	
	for(;;){
		printf("Waiting for USR1 signal\n");
		sleep(1);
    }
}
