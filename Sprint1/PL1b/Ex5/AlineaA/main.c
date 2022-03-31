#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

int main() {
	for(;;){
		printf("I Like Signal\n");
		sleep(1);
	}
}
