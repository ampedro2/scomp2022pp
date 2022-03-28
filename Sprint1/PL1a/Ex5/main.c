#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void) {
	pid_t p2 = -2;
    pid_t p1 = fork();
    int status;
    int p;
    if (p1 > 0){ /* apenas o processo pai dá fork novamente */
		p2 = fork();
	}
    if (p1 == 0){ /* o filho 1 para 1 segundo e retorna o valor 1 */
		sleep(1);
		exit(1);
	}
	else if (p2 == 0 && p1 > 0){ /* o filho 2 para 2 segundos e retorna o valor 2 */
		sleep(2);
		exit(2);
	}
	else{
		p = waitpid(p1, &status, 0); /* espera até o processo filho 1 terminar */
        if (WIFEXITED(status)) {
            printf("%d - %d\n", p, WEXITSTATUS(status)); /* printa o pid do processo filho 1 e o valor que o próprio retornou */
        }
        p = waitpid(p2, &status, 0); /* espera até o processo filho 2 terminar */
        if (WIFEXITED(status)) {
            printf("%d - %d\n", p, WEXITSTATUS(status)); /* printa o pid do processo filho 2 e o valor que o próprio retornou */
        }
	}
    
    return 0;
}
