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

#define NUM_ELEMENTS 10

typedef struct{
	char path[NUM_ELEMENTS][50];
	char wordToSearch[NUM_ELEMENTS][15];
	int numberOfOccurences[NUM_ELEMENTS];
} searchWord;

void inicializeWords(searchWord *sword){
	char path[50];
	char words[10][10] = {"car", "ISEP", "hello", "a", "ola", "and", "SCOMP", "memory", "Porto", "Portugal"};
	for (int i = 0; i < NUM_ELEMENTS; i++){
		sprintf(path, "Files/file%d.txt", i);
		strcpy(sword->path[i], path);
		strcpy(sword->wordToSearch[i], words[i]);
		printf("%s - %s\n", sword->path[i], sword->wordToSearch[i]);
		sword->numberOfOccurences[i] = 0;
	}
}

int main(int argc, char *argv[]) {
	int i, j, k, status, fd, data_size = sizeof(searchWord), r, oc, oct = 0;
	pid_t procid[10];
	searchWord *shared_data;
	FILE *file;
	char strinfOfTheFile[100];
	
	fd = shm_open("/ex13", O_CREAT|O_EXCL|O_RDWR, S_IRUSR|S_IWUSR);
	if(fd == -1){
		printf("Erro a abrir");
		exit(0);
	}
	ftruncate(fd, data_size);
	shared_data = (searchWord *)mmap(NULL, data_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	inicializeWords(shared_data);

	for (i = 0; i < NUM_ELEMENTS; i++){
		procid[i] = fork();
		if (procid[i] == 0){
			if ((file = fopen(shared_data->path[i],"r")) == NULL){
			   printf("Error! opening file");
			   exit(1);
			}
			
			while(!feof(file)){
				while(fgets(strinfOfTheFile, sizeof(strinfOfTheFile), file)){
					for (j = 0; j < strlen(strinfOfTheFile) - strlen(shared_data->wordToSearch[i])+1; j++){
						oc = 0;
						for(k = 0; k < strlen(shared_data->wordToSearch[i]); k++){
							if(strinfOfTheFile[j+k] == shared_data->wordToSearch[i][k]) oc++;
						}
						if(oc == strlen(shared_data->wordToSearch[i])) oct++;
					}
				}
			}
			
			shared_data->numberOfOccurences[i] = oct;
			fclose(file);
			exit(1);
		}
		else{
			waitpid(procid[i], &status, 0);
		}
	}

	for (i = 0; i < NUM_ELEMENTS; i++){
		printf("A palavra %s foi encontrada %d vez(es) no ficheiro %s\n", shared_data->wordToSearch[i], shared_data->numberOfOccurences[i], shared_data->path[i]);
	}

	r = munmap(shared_data, data_size);
	if (r < 0) exit(1);
	r = close(fd);
	if (r < 0) exit(1);
	r = shm_unlink("/ex13");
	if (r < 0) exit(1);
	exit(0);
}
