#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

int main(void){
	char read_msg[256];
	char write_msg[256];
	int up[2];
	int down[2];
	pid_t client;
	
	if(pipe(up) == -1){ /* cria o pipe e verifica se houve um erro */
		perror("Pipe up failed");
		return 1;
	}
	if(pipe(down) == -1){ /* cria o pipe e verifica se houve um erro */
		perror("Pipe down failed");
		return 1;
	}
	client = fork();
	if(client > 0){
		close(up[1]); /* fecha a extremidade não usada (write) */
		read(up[0],&read_msg, sizeof(read_msg)); /* lê dados do pipe */
		printf("Mensagem recebida do cliente: %s\n", read_msg);
		close(up[0]); /* fecha a extremidade read */
		
		for (int i = 0; read_msg[i]!='\0'; i++) {
			if(read_msg[i] >= 'a' && read_msg[i] <= 'z') {
				read_msg[i] = read_msg[i] - 32;
			}
			else if (read_msg[i] >= 'A' && read_msg[i] <= 'Z'){
				read_msg[i] = read_msg[i] + 32;
			}
	    }
		
		close(down[0]); /* fecha a extremidade não usada (read) */
		write(down[1],&read_msg, sizeof(read_msg)); /* lê dados do pipe */
		printf("Mensagem enviada (server): %s\n", read_msg);
		close(down[1]); /* fecha a extremidade write */
	}
	else{
		close(up[0]); /* fecha a extremidade não usada (read) */
		printf("Mensagem a ser enviada: ");
		scanf("%[^\n]s", write_msg);
		write(up[1],&write_msg, sizeof(write_msg)); /* lê dados do pipe */
		printf("Mensagem enviada (cliente): %s\n", write_msg);
		close(up[1]); /* fecha a extremidade write */
		
		close(down[1]); /* fecha a extremidade não usada (write) */
		read(down[0],&read_msg, sizeof(read_msg)); /* lê dados do pipe */
		printf("Mensagem recebida do server: %s\n", read_msg);
		close(down[0]); /* fecha a extremidade read */
		exit(10);
	}
}
