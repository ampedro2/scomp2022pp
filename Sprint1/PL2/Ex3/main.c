#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

int main(void){
	char read_msg[20];
	char write_msg[20];
	int fd[2];
	pid_t procid;
	int p, status;
	
	if(pipe(fd) == -1){ /* cria o pipe e verifica se houve um erro */
		perror("Pipe failed");
		return 1;
	}
	procid = fork();
	if(procid > 0){
		close(fd[0]); /* fecha a extremidade não usada (read) */
		strcpy(write_msg, "Hello World");
		write(fd[1],&write_msg,sizeof(write_msg)); /* escreve dados do pipe */
		strcpy(write_msg, "Goodbye!");
		write(fd[1],&write_msg,sizeof(write_msg)); /* escreve dados do pipe */
		close(fd[1]); /* fecha a extremidade write */
	}
	else{
		close(fd[1]); /* fecha a extremidade não usada (write) */
		read(fd[0],&read_msg, sizeof(read_msg)); /* lê dados do pipe */
		printf("%s\n", read_msg);
		read(fd[0],&read_msg, sizeof(read_msg)); /* lê dados do pipe */
		printf("%s\n", read_msg);
		close(fd[0]); /* fecha a extremidade read */
		exit(10);
	}
	
	p = waitpid(procid, &status, 0); /* espera até o processo filho 1 terminar */
	if (WIFEXITED(status)) {
		printf("%d - %d\n", p, WEXITSTATUS(status)); /* printa o pid do processo filho e o valor que o próprio retornou */
	}
}
