#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <semaphore.h>
#include <wait.h>
#include <string.h>

#define NUM_ELEMENTS 200

int main(int argc, char *argv[]) {
	FILE * numbers;
	FILE * output;
	int i = 0, j = 0, num;
	pid_t procid;
    
	sem_t *sem = sem_open("/Ex1", O_CREAT|O_EXCL, 0644, 1);
	if(sem == -1){
		perror("Error in semaphore creation\n");
		exit(0);
	}
	
	for (i = 0; i < 8; i++){
		procid = fork();
		if(procid == -1){
			perror("Error in process creation\n");
			exit(0);
		}
		if (procid == 0){
			sem_wait(sem);
			numbers = fopen("Numbers.txt", "r");
			output = fopen("Output.txt", "a");
            for (j = 0; j < NUM_ELEMENTS; j++){
                fscanf(numbers,"%d",&num);
                fprintf(output,"%d\n",num);
            }
			
			fclose(numbers);
			fclose(output);
			sem_post(sem);
			exit(1);
		}
	}
	int status;
	for(i = 0; i < 8; i++){
		wait(&status);
	}
	output = fopen("Output.txt", "r");
	for (i = 0; i < NUM_ELEMENTS*8; i++){
		fscanf(output, "%d\n", &num);
		printf("%d - %d\n", i+1, num);
		if ((i+1)%200 == 0){
			printf("-----------\n");
		}
	}
	
	if (sem_close(sem) == -1)
    {
        perror("Error closing the semaphore\n");
        exit(0);
    }

    if (sem_unlink("/Ex1") == -1)
    {
        perror("Error removing the semaphore.\n");
        exit(0);
    }
	
	return 0;
}
