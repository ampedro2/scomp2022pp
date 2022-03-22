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
    if (p1 > 0){
		p2 = fork();
	}
    if (p1 == 0){
		sleep(1);
		exit(1);
	}
	else if (p2 == 0 && p1 > 0){
		sleep(2);
		exit(2);
	}
	else{
		p = waitpid(p1, &status, 0);
        if (WIFEXITED(status)) {
            printf("%d - %d\n", p, WEXITSTATUS(status));
        }
        p = waitpid(p2, &status, 0);
        if (WIFEXITED(status)) {
            printf("%d - %d\n", p, WEXITSTATUS(status));
        }
	}
    
    return 0;
}
