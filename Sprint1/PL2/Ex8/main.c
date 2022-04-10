#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

typedef struct {
	
 char mensagem[80];
 
 int ronda;
 
} jogo;

int criar(int filhos){
	
    pid_t pid;
    
    int i;
    
    for (i = 0; i < filhos; i++){
		
        pid = fork();
        
        if (pid < 0) {
			
          return -1;
          
        } 
        
        else if (pid == 0) {
			
          return i + 1;
          
        }
        
    }
    
    return 0;
}

int main(void) {

    int fd[2];

    if (pipe(fd) == -1){
		
        perror("Pipe up failed");
        
        return 1;
    }

    int id = criar(10);

    if (id == 0) {

        close(fd[0]);
        
        int i;

        jogo pipegame;

        for (i = 0; i < 10; i++){

          strcpy(pipegame.mensagem, "Win");
          
          pipegame.ronda = i + 1;

          write(fd[1], &pipegame, sizeof(pipegame));

          sleep(2);
          
        }

        close(fd[1]);
    }


    if (id > 0) {

      close(fd[1]);

      jogo infogame;

      read(fd[0], &infogame, sizeof(infogame));

      close(fd[0]);
        printf("Son %d won round %d\n", id, infogame.ronda);
        exit(infogame.ronda);
    }

    int status;
    
    int j;

    for (j = 0; j < 10; j++) {
		
        pid_t pid = wait(&status);
        
        if (WIFEXITED(status)) {
 
            int rondavitoria = WEXITSTATUS(status);
            
            printf("Son with PID %d won round %d\n", pid, rondavitoria);
        }
    }
    
    return 0;
}
