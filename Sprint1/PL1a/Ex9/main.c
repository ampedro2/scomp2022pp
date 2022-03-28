#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

int main ()
{
	pid_t array[10];
	int i, j;
	
	for (i = 0; i < 10; i++){
		array[i] = fork(); /* criação do processo filho (10 vezes devido ao ciclo) */
		
		if (array[i] == 0){ /* cada processo filho irá printar 100 valores */
			for (j = i*100+1; j < (i+1)*100+1; j++){
				printf("%d - ", j);
			}
			printf("\n");
			exit(0);
		}
	}
	
	for (i = 0; i < 10; i++){
		wait(NULL); /* O processo pai só termina quando todos os seus processos filhos terminarem */
	}
				
} 

