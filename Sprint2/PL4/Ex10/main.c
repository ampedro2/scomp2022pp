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

    int menu() {
        int opt;

        printf("1. Consultar\n");
        printf("2. Inserir\n");
        printf("3. Consultar tudo\n");
        printf("0. Sair\n");

        scanf("%d", &opt);

        return opt;
    }

    void runProgram(char *programa) {

        pid_t pid = fork();

        if (pid == 0) {
            execlp(programa, programa, (char *)NULL);
            exit(1);
        }
        while (wait(NULL) > 0) {
        
        };
    }


int main(int argc, char *agrv[]) {

    int fd, semR;
    int data_size = sizeof(personalDataRecords);

    fd = shm_open("/share_data", O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        perror("Erro a abrir a memoria partilhada.");
        exit(1);
    }

    ftruncate(fd, data_size);

    personalDataRecords *sharedData = (personalDataRecords *)mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    sem_t *sem;

    if ((sem = sem_open("semaforo", O_CREAT | O_EXCL, 0644, 1)) == SEM_FAILED) {
        perror("Error in sem_open()");
        exit(1);
    }

    char str[300];
    char margem[100];
    strcpy(str,"________________MAIN_____________\n");
    strcpy(margem,"_________________________________________\n");
    puts(margem);
    puts(str);
    puts(margem);
    semR = sem_wait(sem);
    if ( semR == -1) {
       perror("Erro sem_wait()!");
       exit(EXIT_FAILURE);
    }

    int i;
    for(i = 0; i < MAX_RECORDS; i++) {
        sharedData->dataRecords[i].number = 0;
    }

    semR = sem_post(sem);
    if ( semR == -1) {
       perror("Erro sem_post()!");
       exit(EXIT_FAILURE);
    }

    int opt;

    do {
        opt = menu();

        switch (opt) {
            case 1:
                runProgram("./consult.o");
                break;
            case 2:
                runProgram("./insert.o");
                break;
            case 3:
                runProgram("./consult_all.o");
                break;
            case 0:
                return 0;
            default:
                printf("\nErro na opção selecionada");
                return 0;
        }
    } while(opt == 1 || opt == 2 || opt == 3);

    fd = munmap(sharedData, data_size);
    if ( fd < 0) {
        perror("Error at munmap()!!!\n");
        exit(1);
    }
    fd = close(fd);
    if ( fd < 0) {
        perror("Erro a fechar!!!\n");
        exit(1);
    }
    fd = shm_unlink("/share_data");
    if ( fd < 0) {
       perror("Erro shm_unlink()!!!\n");
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
