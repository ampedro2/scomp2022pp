#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

#define CMD 2 /* definição do número de processos e comandos criados */

typedef struct {
 char cmd[32];
 int tempo;
} comando;

void inicializeArray(comando *array){
	for (int i = 0; i < CMD; i++){ /* preenchimento do array com os comandos e os seus tempos máximos de execução (random) */
		strcpy(array[i].cmd,"./ProgramTest");
		time_t t;
		srand((int)time(&t) % getpid());
		array[i].tempo = rand() % 10;
		printf("%s - %d second(s)\n", array[i].cmd, array[i].tempo);
	}
}

int main(int argc, char *argv[]){
	pid_t procid;
	comando array[CMD];
	
	inicializeArray(array);
	for (int i = 0; i < CMD; i++){
		procid = fork();
		time_t t;
		srand((int)time(&t) % getpid());
		if (procid > 0){
			waitpid(procid, NULL, 0);
		}
		else if (procid == 0){
			alarm(array[0].tempo + 1); /* caso o tempo de execução do programa ProgramTest ultrapasse o tempo máximo definido, é lançado o sinal ALRM, que é tratado no outro programa */
			execl(array[i].cmd, array[i].cmd, (char*)NULL); /* é executado o comando ./ProgramTest e o processo é segue a sua execução no programa ProgramTest */
			exit(0);
		}
		else{
			printf("Something went wrong.\n");
		}
	}
	
}
