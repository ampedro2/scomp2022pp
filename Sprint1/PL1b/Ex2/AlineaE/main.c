#include <stdio.h>
#include <stdlib.h>

int main() {
	pid_t idp;
	int sig;
	printf("PID do processo que irá receber o signal: ");
	scanf("%d", &idp);
	printf("Valor do signal: ");
	scanf("%d", &sig);
	kill(idp,sig);
}
