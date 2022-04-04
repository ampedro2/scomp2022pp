#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

int main(void){
	pid_t read_msg;
	pid_t write_msg;
	int fd[2];
	pid_t pid;
	
	if(pipe(fd) == -1){ /* cria o pipe e verifica se houve um erro */
		perror("Pipe failed");
		return 1;
	}
	pid = fork();
	if(pid > 0){
		close(fd[0]); /* fecha a extremidade não usada (read) */
		write_msg = getpid();
		printf("Pid do pai: %d\n", write_msg); /* escreve no pipe */
		write(fd[1],&write_msg,sizeof(int));
		close(fd[1]); /* fecha a extremidade write */
	}
	else{
		close(fd[1]); /* fecha a extremidade não usada (write) */
		read(fd[0],&read_msg, sizeof(int)); /* lê dados do pipe */
		printf("Pid que o filho leu: %d\n", read_msg);
		close(fd[0]); /* fecha a extremidade read */
	}
}
