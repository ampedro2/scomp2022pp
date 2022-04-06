#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

#define NUM 50

typedef struct{
	int customer_code;
	int product_code;
	int quantity;
} sale;


void inicializeArray(sale *sales){
	srand ( time(NULL) );
	for (int i = 0; i < NUM; i++){ /* atribuir valor aos dois vetores */
		sales[i].customer_code = rand() % NUM;
		sales[i].product_code = rand() % NUM;
		sales[i].quantity = rand() % 100;
		printf("%d - %d\n", sales[i].product_code, sales[i].quantity);
	}
}

int main(void){
	int pp[2];
	sale sales[NUM];
	int products[NUM];
	pid_t procid;
	int i, j, product_code, status, aux=0;
	
	inicializeArray(sales);
	if (pipe(pp) == -1){ /* caso a criação do pipe dê erro, é imprimida a mensagem 'Pipe up failed' e o processo acaba */
		perror("Pipe up failed\n");
		exit(0);
	}
		
	for (i = 0; i < 10; i++){
		procid = fork();
		if (procid == 0){
			close(pp[0]); /* fecha a extremidade do pipe não usada, neste caso, a de leitura */
			for (j = i*(NUM/10); j < (i+1)*(NUM/10); j++){
				if (sales[j].quantity > 20){
					product_code = sales[j].product_code; /* guarda na variavel product_code os codigos dos produtos que venderam mais de 20 unidades */
					write(pp[1],&product_code,sizeof(int)); /* escreve no pipe, de modo a que o processo pai receba o pointer para o vetor soma */
				}
			}
			close(pp[1]); /* fecha a extremidade do pipe usada, neste caso, a de escrita */
			exit(1);
		}
		else{
			waitpid(procid, &status, 0); /* Espera e verifica se a conclusão do processo ocorreu normalmente */ 
			if (!WIFEXITED(status)) {
                perror("Process not finished\n");
                return 0;
            }
		}
	}
	printf("--- Resultado final ---\n");
	close(pp[1]); /* fecha a extremidade do pipe não usada, neste caso, a de escrita */
	while (read(pp[0],&product_code,sizeof(int))){
		products[aux] = product_code;
		aux++;
	}
	close(pp[0]); /* fecha a extremidade do pipe usada, neste caso, a de leitura */
	
	for (i = 0; i < aux; i++){
		printf("O produto com o código %d teve mais que 20 unidades vendidas.\n", products[i]);
	}
}
