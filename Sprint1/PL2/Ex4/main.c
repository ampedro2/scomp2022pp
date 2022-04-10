#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>

int main(){

    int fd[2];
    
    char palavra[100];
    
    if(pipe(fd) == -1){
		
        printf("Pipe failed");
        
        return -1;
    }

    pid_t p = fork();

    if (p > 0) {

        char *filename = "output.txt";
        
        FILE* fp = fopen(filename, "r");

        while (fgets(palavra, 100, fp));
        
        fclose(fp);

        close(fd[0]);

        write(fd[1], &palavra, sizeof(palavra));
        
        close(fd[1]);

        wait(NULL);

    } 
    else if (p == 0) {
		
        close(fd[1]);

        read(fd[0], &palavra, sizeof(palavra));
        
        close(fd[0]);

        printf("String --> %s\n", palavra);

        exit(0);
    }

    return 0;
}
