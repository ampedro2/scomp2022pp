#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

#define NUM 1000

void inicializeArrays(int *vec1, int *vec2){
	srand ( time(NULL) );
	for (int i = 0; i < NUM; i++){ /* atribuir valor aos dois vetores */
		vec1[i] = rand() % 100;
		vec2[i] = rand() % 100;
		printf("%d: %d + %d\n", i, vec1[i], vec2[i]);
	}
}

int main(void){
	int pipes[5][2];
	int soma[200];
	int total[NUM];
	int status, i, j;
	pid_t procid;
	int vec1[NUM];
	int vec2[NUM];
	
	inicializeArrays(vec1, vec2);
	for (i = 0; i < 5; i++){
		if (pipe(pipes[i]) == -1){ /* caso a criação do pipe dê erro, é imprimida a mensagem 'Pipe up failed' e o processo acaba */
			perror("Pipe up failed\n");
			exit(0);
		}
		procid = fork();
		if (procid == 0){
			close(pipes[i][0]); /* fecha a extremidade do pipe não usada, neste caso, a de leitura */
			for (j = i*200; j < (i+1)*200; j++){
				soma[j - i*200] = vec1[j] + vec2[j]; /* é guardada no vetor soma o resultado da soma das posições homólogas nos dois vetores */
			}
			write(pipes[i][1],soma,sizeof(soma)); /* escreve no pipe, de modo a que o processo pai receba o pointer para o vetor soma */
			close(pipes[i][1]); /* fecha a extremidade do pipe usada, neste caso, a de escrita */
			exit(1);
		}
		else{
			waitpid(procid, &status, 0); /* Espera e verifica se a conclusão do processo ocorreu normalmente */ 
			if (!WIFEXITED(status)) {
                perror("Pipe up failed\n");
                return 0;
            }
		}
	}
	
	printf("--- Resultado final ---\n");
	for (i = 0; i < 5; i++){
		close(pipes[i][1]); /* fecha a extremidade do pipe não usada, neste caso, a de escrita */
		read(pipes[i][0],soma,sizeof(soma)); /*lê, dos pipes em que cada um está associado a um processo, o vetor soma que contém a soma das posições homólogas nos dois vetores num determinado intervalo */
		for (j = 0; j < 200; j++){
			total[j+i*200] = soma[j];
			printf("%d - %d\n", j+i*200, total[j+i*200]);
		}
		close(pipes[i][0]); /* fecha a extremidade do pipe usada, neste caso, a de leitura */
	}
}
