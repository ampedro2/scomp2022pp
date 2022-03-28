#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

int main ()
{
	pid_t array[5], child;
	int i, j, max_value = 0, status;
	int numbers[1000], result[1000];
	
	for (i = 0; i < 1000; i++){ /* preenchimento do vetor numbers com números aleatórios de 0 a 255 */
		numbers[i] = rand () % 256;
		printf("%d - %d\n", i, numbers[i]);
	}
	
	for (i = 0; i < 5; i++){ /* criação de 5 processos que irão analisar cada um 1/5 do vetor e calcular nesse intervalo o valor máximo */
		array[i] = fork();
		
		if (array[i] == 0){
			for (j = i*200; j < (i+1)*200; j++){
				if (max_value < numbers[j]){
					max_value = numbers[j];
				}
			}
			exit(max_value); /* retorno do valor máximo no intervalo específico */
		}
	}
	
	for (i = 0; i < 5; i++){
		waitpid(array[i], &status, 0);
		if (WIFEXITED(status)){
			printf("Processo: %d - Máximo: %d\n", array[i], WEXITSTATUS(status)); /* printa o valor máximo que cada processo filho determinou no respetivo intervalo */
			if (max_value < WEXITSTATUS(status)){
				max_value = WEXITSTATUS(status); /* o maior valor dos valores printados acima irá ser o maior valor do vetor numbers */
			}
		}
	}
	printf("Máximo: %d\n", max_value);
	child = fork(); /* criação de mais um processo filho */
	if (child == 0){
		for (i = 0; i < 500; i++){
			result[i]=((int) numbers[i]/max_value)*100; /* cada vez o maior valor calculado anteriormente é encontrado na primeira metade do vetor, é printado o valor 100 */
			printf("%d - %d\n", i, result[i]);
		}
	}
	else if (child > 0){
		waitpid(child, &status, 0);
		for (i = 500; i < 1000; i++){
			result[i]=((int) numbers[i]/max_value)*100; /* cada vez o maior valor calculado anteriormente é encontrado na segunda metade do vetor, é printado o valor 100 */
			printf("%d - %d\n", i, result[i]);
		}
	}
				
} 

