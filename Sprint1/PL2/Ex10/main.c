#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

int nbrGenerator();

int main(void) {

    int saldo = 20;

    int notification = 1;

    int aposta;

    int fd1[2];
    
    printf("Wallet = 20\n");
    
    if (pipe(fd1) == -1){
		
        perror("Pipe 1 failed");
        
        return 1;
    }

    int fd2[2];
    
    if (pipe(fd2) == -1){
		
        perror("Pipe 2 failed");
        
        return 1;
    }

    pid_t pid = fork();

      if (pid > 0) {
		  
		  srand(time(NULL));

          close(fd1[0]);

          close(fd2[1]);

          int rndNumber;
          
          while(saldo > 0) {
			  
            printf("\n\n");
            
            sleep(1);
            
            rndNumber = nbrGenerator();
            
            printf("Number = %d",rndNumber);
            
            if (saldo > 0) {
				
              notification = 1;
              
            } 
            
            else {
				
              notification = 0;
              
            }

            write(fd1[1], &notification, sizeof(notification));

            if (notification == 1) {
				
              read(fd2[0], &aposta, sizeof(aposta));
              
              printf("\nBet = %d\n",aposta);
              
            }
            
            if (aposta == rndNumber) {
				
              saldo += 10;
              
            }
            
            else {
				
              saldo -= 5;
              
            }
 
            write(fd1[1], &saldo, sizeof(saldo));
          }

          close(fd1[1]);

          close(fd2[0]);
      }


      if (pid == 0) {
		  
        srand(time(NULL) * getpid());

        close(fd1[1]);
  
        close(fd2[0]);
        
        while (notification != 0) {

          read(fd1[0], &notification, sizeof(notification));
          
          if (notification != 0) {

            aposta = nbrGenerator();

            write(fd2[1], &aposta, sizeof(aposta));
          }

          read(fd1[0], &saldo, sizeof(saldo));
          
          if(saldo != 0){
			  
			printf("Wallet = %d\n",saldo);
			
		}
          
          
        }

        close(fd1[0]);

        close(fd2[1]);
        
        exit(1);
        
      }

    return 0;
}

int nbrGenerator(){
	
    int up = 5;
    
    int down = 0;

    int rndNumber = (int) (rand() % (up - down + 1)) + down;
    
    return rndNumber;
}		
