#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <errno.h>

    #define CHAR_MAX_NAME 30
    #define CHAR_MAX_ADDRESS 100
    #define MAX_RECORDS 100

    typedef struct {
        int number;
        char name[CHAR_MAX_NAME];
        char address[CHAR_MAX_ADDRESS];
    } personalData;

    typedef struct {
        personalData dataRecords[MAX_RECORDS];
    } personalDataRecords;

int main(int argc, char *agrv[]) {

    int fd,semR;
    int data_size = sizeof(personalDataRecords);

    fd = shm_open("/share_data", O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        perror("Erro a abrir a memoria partilhada.");
        exit(1);
    }

    ftruncate(fd, data_size);

    personalDataRecords *sharedData = (personalDataRecords *)mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    sem_t *sem;

    if ((sem = sem_open("semaforo", 0)) == SEM_FAILED) {
        perror("Error in sem_open()");
        exit(1);
    }

    char str[300];
    char notFound[40];
    char margem[100];
    strcpy(notFound,"NO MATCH!!!\n");
    strcpy(str,"_____________CONSULT_RECORD______________\n");
    strcpy(margem,"_________________________________________\n");
    puts(margem);
    puts(str);
    puts(margem);
    int number,i;
    printf("Insert the number to search:\n");
    scanf("%d", &number);
    fflush(stdin);
    semR = sem_wait(sem);
    if ( semR == -1) {
       perror("Erro sem_wait()!");
       exit(1);
    }

    for(i = 0; i < MAX_RECORDS; i++) {
      if (number == sharedData->dataRecords[i].number ) {
        puts(margem);
        sprintf(str,"Numero -> %d\nNome -> %s\nMorada -> %s", sharedData->dataRecords[i].number, sharedData->dataRecords[i].name, sharedData->dataRecords[i].address);
        puts(str);
        puts(margem);
      }
    }

    if(strcmp (str, "_____________CONSULTAR_REGISTO______________\n" ) == 0){
      puts(margem);
      puts(notFound);
      puts(margem);
    }

    semR = sem_post(sem);
    if ( semR == -1) {
       perror("Erro sem_post()!");
       exit(1);
    }

    semR = sem_close(sem);
    if ( semR < 0) {
        perror("Erro sem_close()!\n");
        exit(1);
    }
    semR = sem_unlink("semaforo");
    if ( semR != 0) {
        int error = errno;
        printf("Error: %s", strerror(error));
        printf("Erro shm_unlink() do semaforo!\n");
        exit(1);
    }

    return 0;
}
