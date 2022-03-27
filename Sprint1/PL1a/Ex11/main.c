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
	int numbers[1000], result[500];
	
	for (i = 0; i < 1000; i++){
		numbers[i] = rand () % 256;
		printf("%d - %d\n", i, numbers[i]);
	}
	
	for (i = 0; i < 5; i++){
		array[i] = fork();
		
		if (array[i] == 0){
			for (j = i*200; j < (i+1)*200; j++){
				if (max_value < numbers[j]){
					max_value = numbers[j];
				}
			}
			exit(max_value);
		}
	}
	
	for (i = 0; i < 5; i++){
		waitpid(array[i], &status, 0);
		if (WIFEXITED(status)){
			printf("Processo: %d - Máximo: %d\n", array[i], WEXITSTATUS(status));
			if (max_value < WEXITSTATUS(status)){
				max_value = WEXITSTATUS(status);
			}
		}
	}
	printf("Máximo: %d\n", max_value);
	child = fork();
	if (child == 0){
		for (i = 0; i < 500; i++){
			result[i]=((int) numbers[i]/max_value)*100;
			printf("%d - %d\n", i, result[i]);
		}
	}
	else if (child > 0){
		waitpid(child, &status, 0);
		for (i = 500; i < 1000; i++){
			result[i]=((int) numbers[i]/max_value)*100;
			printf("%d - %d\n", i, result[i]);
		}
	}
				
} 

