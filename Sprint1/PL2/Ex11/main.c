#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

#define NUM 50

int main(void){
	int pp[6][2];
	pid_t procid;
	time_t t;
	int i, max_calculated, max_received;
	
	for (i = 0; i < 6; i++){ /* ciclo necessário para criar o sexto pipe (último filho -> parent) */
		if (pipe(pp[i]) == -1){ 
			perror("Pipe up failed\n");
			exit(0);
		}
	}
		
	for (i = 0; i < 5; i++){
		procid = fork();
		if (procid == 0){
			close(pp[i][1]);
			read(pp[i][0],&max_received, sizeof(int)); /* lê do pipe que contém o número do processo anterior */
			close(pp[i][0]);
			srand((int)time(&t) % getpid());
			max_calculated = rand() % 500 + 1;
			printf("PID %d -> %d\n", getpid(), max_calculated);
			if (max_calculated < max_received){ /* caso o número gerado pelo processo anterior for maior que o gerado pelo processo presente, está guarda na variável max_calculated esse valor, de modo a que sempre escreva no pipe seguinte o maior entre os dois valores */
				max_calculated = max_received;
			}
			close(pp[i+1][0]); 
			write(pp[i+1][1],&max_calculated,sizeof(int)); /* escreve no pipe o número que o processo seguinte irá ler */
			close(pp[i+1][1]);
			exit(1);
		}
	}
	
	srand((int)time(&t) % getpid());
	max_calculated = rand() % 500 + 1;
	printf("PID pai %d -> %d\n", getpid(), max_calculated);
	close(pp[0][0]); 
	write(pp[0][1],&max_calculated,sizeof(int)); /* como a função read é bloqueante, o primeiro processo terá que esperar que o pai dê write no respetivo pipe para que o o processo continue */
	close(pp[0][1]);
	
	close(pp[5][1]); 
	read(pp[5][0],&max_received,sizeof(int)); /* recebe do último processo o que será o maior valor gerado entre todos os processos */
	close(pp[5][0]);
	printf("Máximo calculado: %d\n", max_received);
}
