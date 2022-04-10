#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

#define NUM_PIECES 1000

int main(void){
	int pp[5][2];
	int pieces = 0, totalPieces = 0, piecesReceived = 0;
	pid_t procid[5];
	int i, status;


	for (i = 0; i < 5; i++){ /* ciclo necessário para criar o sexto pipe (último filho -> parent) */
		if (pipe(pp[i]) == -1){ 
			perror("Pipe up failed\n");
			exit(0);
		}
	}
		
	for (i = 0; i < 4; i++){
		procid[i] = fork();
		if(procid[i] == 0){ 
			if (i == 0){
				printf("----- M1 -----\n");
				pieces = 5;
				close(pp[i+1][0]);
				while(totalPieces < NUM_PIECES){
					totalPieces += 5;
					write(pp[i+1][1], &pieces,sizeof(int));
					printf("M1 sent %d pieces to M2\n", pieces);
				}
				close(pp[i+1][1]);
				exit(0);
			}
			else if (i == 1){
				printf("----- M2 -----\n");
				pieces = 5;
				close(pp[i][1]);
				close(pp[i+1][0]);
				while(totalPieces < NUM_PIECES){
					read(pp[i][0],&piecesReceived,sizeof(piecesReceived));
					totalPieces += piecesReceived;
					printf("M2 received %d pieces from M1\n", piecesReceived);
					write(pp[i+1][1], &piecesReceived,sizeof(int));
					printf("M2 sent %d pieces to M3\n", pieces);
				}
				close(pp[i][0]);
				close(pp[i+1][1]);
				exit(0);
			}
			else if (i == 2){
				printf("----- M3 -----\n");
				pieces = 10;
				close(pp[i][1]);
				close(pp[i+1][0]);
				while(totalPieces < NUM_PIECES){
					read(pp[i][0],&piecesReceived,sizeof(piecesReceived));
					totalPieces += piecesReceived;
					printf("M3 received %d pieces from M2\n", piecesReceived);
					if (totalPieces % 10 == 0){
						write(pp[i+1][1], &pieces,sizeof(int));
						printf("M3 sent %d pieces to M4\n", pieces);
					}
				}
				close(pp[i][0]);
				close(pp[i+1][1]);
				exit(0);
			}
			else if (i == 3){
				printf("----- M4 -----\n");
				pieces = 100;
				close(pp[i][1]);
				close(pp[i+1][0]);
				while(totalPieces < NUM_PIECES){
					read(pp[i][0],&piecesReceived,sizeof(piecesReceived));
					totalPieces += piecesReceived;
					printf("M4 received %d pieces from M3\n", piecesReceived);
					if (totalPieces % 100 == 0){
						write(pp[i+1][1], &pieces,sizeof(int));
						printf("M4 sent 100 pieces to A1\n");
					}
				}
				close(pp[i][0]);
				close(pp[i+1][1]);
				exit(0);
			}
		}
		else{
			waitpid(procid[i],&status,0);
		}
	}
	
	close(pp[4][1]); 
	for (i = 0; i < 10; i++){
		read(pp[4][0],&piecesReceived,sizeof(int));
		totalPieces += piecesReceived;
		printf("A1 received %d pieces from M4\n", piecesReceived);
	}
	close(pp[4][0]);
	
	printf("Total pieces received by A1 from M4: %d\n", totalPieces);

	return 0; 
}
