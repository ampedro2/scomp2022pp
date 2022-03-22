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
	if (p1 == 0){
		for (i = 0; i < ARRAY_SIZE/2-1; i++){
			if (n == numbers[i]){
				ntimes++;
			}
		}
		exit(ntimes);
	}
	else if (p1 > 0){
		for (i = ARRAY_SIZE/2; i < ARRAY_SIZE-1; i++){
			if (n == numbers[i]){
				ntimes++;
			}
		}
		printf("Number of times parent process found n: %d\n", ntimes);
		waitpid(p1, &status, 0);
        if (WIFEXITED(status)) {
            ntimes += WEXITSTATUS(status);
        }
		printf("Number of times child process found n: %d\n", WEXITSTATUS(status));
		printf("Number of times n was found: %d\n", ntimes);
	}
} 

