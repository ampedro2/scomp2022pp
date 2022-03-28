#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

#define ARRAY_SIZE 1000
int main ()
{
	int numbers[ARRAY_SIZE]; /* array to lookup */
	int n; /* the number to find */
	time_t t; /* needed to init. the random number generator (RNG) */
	int i;
	int ntimes = 0;
	int status;

	/* intializes RNG (srand():stdlib.h; time(): time.h) */
	srand ((unsigned) time (&t));

	/* initialize array with random numbers (rand(): stdlib.h) */
	for (i = 0; i < ARRAY_SIZE; i++)
	numbers[i] = rand () % 10000;

	/* initialize n */
	n = rand () % 10000;
	
	pid_t p1 = fork();
	if (p1 == 0){ /* caso o processo seja o filho, analisa a primeira metade do vetor numbers e retorna o número de vezes que o valor n foi encontrado nessa metade */
		for (i = 0; i < ARRAY_SIZE/2-1; i++){
			if (n == numbers[i]){
				ntimes++;
			}
		}
		exit(ntimes);
	}
	else if (p1 > 0){ /* caso o processo seja o filho, analisa a primeira metade do vetor numbers e determina o número de vezes que o valor n foi encontrado nessa metade */
		for (i = ARRAY_SIZE/2; i < ARRAY_SIZE-1; i++){
			if (n == numbers[i]){
				ntimes++;
			}
		}
		printf("Number of times parent process found n: %d\n", ntimes);
		waitpid(p1, &status, 0); /*aguarda que o processo filho acabe, uma vez que necessita do valor retornado por ele para calcular o resultado final */
        if (WIFEXITED(status)) {
            ntimes += WEXITSTATUS(status);
        }
		printf("Number of times child process found n: %d\n", WEXITSTATUS(status)); /*printa o número de vezes que o processo filho encontrou o valor n na primeira metade do vetor numbers */
		printf("Number of times n was found: %d\n", ntimes); /*printa o valor total de vezes que o valor n foi encontrado no vetor numbers */
	}
} 

