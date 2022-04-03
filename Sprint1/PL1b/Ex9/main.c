#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

volatile sig_atomic_t var = 0;

void USR1Function (int signo, siginfo_t *sinfo, void *context) {
	printf("The child process %d received URS1 signal from the parent process %d.\n", getpid(), getppid());
}

void taskA() {
	printf("Process %d - Task A.\n", getpid());
	for (int i = 1; i <= 3; i++){ /* A cada iteração do ciclo, imprime-se a quantidade de segundos que passou */
		sleep(1);
		printf("%d second(s).\n", i);
	}
}

void taskB() {
	time_t t;
    srand((unsigned) time(&t));
	int numberOfSeconds = rand () % 5 + 1; /* Gerar um número random de 1 a 5 -> (0 a 4) + 1 */
	sleep(numberOfSeconds);
	printf("Process %d - Task B - %d second(s)\n", getpid(), numberOfSeconds);
}

void taskC() {
	printf("Process %d - Task C.\n", getpid());
}

int main(int argc, char *argv[]){
	pid_t p;
	p = fork();
	
	if (p>0){ /* Caso seja o pai, a taskA é executada e é enviado o sinal com o número 10 (SIGUSR1) para o filho, cujo pid está guardado na variável p */
		taskA();
		var = 1;
		kill(p, 10);
	}
	else if (p == 0){ /* Caso seja o filho, declara-se uma função handle para o reconhecimento do sinal SIGUSR1 */
		struct sigaction act;
		memset(&act, 0, sizeof(struct sigaction));
		sigemptyset(&act.sa_mask);
		act.sa_sigaction = USR1Function;
		sigaction(SIGUSR1, &act, NULL);
		
		taskB();
		while (var == 0){ /* O processo filho fica à espera que o processo pai atualize a variável volátil var aquando do término da execução da taskA e assim, executa-se a função taskC*/
			sleep(1);
		}
		taskC();
	}
	else{ /* Caso a função fork() retorne uma valor abaixo de 0, é indicativo que ocorreu algum erro durante a execução da mesma */
		printf("Something went wrong\n");
	}
}
