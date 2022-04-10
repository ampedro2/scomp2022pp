#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

int pipearray[6][2];

typedef struct {
 int referencia;
 int pipeInfo;
} barcodeReader;

typedef struct {
 int referencia;
 float price;
 char description[30];
} productInfo;

int main(void) {
    int i;
    pid_t pid;

    for (i = 0; i < 6; i++) {
		
        if (pipe(pipearray[i]) == -1){
			
            perror("Pipe failed");
            
            return 1;
            
        }
        
    }
    
    char arr[10];

    printf("\nType product reference:");
    
    for (i = 0; i < 5 && fgets(arr, 10, stdin) != NULL; i++) {

      pid = fork();

      if(pid == 0){

      int productref;

      barcodeReader info;

      productInfo infoAsked;

      close(pipearray[0][0]);
	
      close(pipearray[i+1][1]);
        
      productref = atoi(arr);
        
      info.referencia = productref;
        
      info.pipeInfo = i;
        
      write(pipearray[0][1], &info, sizeof(info));
        
      read(pipearray[i+1][0], &infoAsked, sizeof(infoAsked));
        
      if (infoAsked.price == -1.0) {
              
              printf("Product not found.\n");
              
              } 
              
              else {
                      
                 printf("The product with the reference %d costs %.2f . Product description --> '%s';\n", infoAsked.referencia ,infoAsked.price ,infoAsked.description);
                    
              }

            if (i == 4) {

              close(pipearray[0][1]);
              printf("\n");
            }

            close(pipearray[i+1][0]);
            
            exit(i);
          }
          
          if(i != 4){
			  
            printf("\nType product reference:");
            
          }

    }


    close(pipearray[0][1]);

    barcodeReader infoBarcodeReader;

    productInfo foundProduct;

    productInfo baseDados[10] = {
      {1,1,"A"},
      {2,2,"B"},
      {3,3,"C"},
      {4,4,"D"},
      {5,5,"E"},
      {6,6,"F"},
      {7,7,"G"},
      {8,8,"h"}, };

      for(i = 0; i < 5; i++){

          close(pipearray[i+1][0]);

          read(pipearray[0][0], &infoBarcodeReader, sizeof(infoBarcodeReader));

          int ref = infoBarcodeReader.referencia;
          
          productInfo notFoundProduct = {-1,-1.0,"NaN"};
          
          int flagFound = -1;

            for (int j = 0; j < 10; j++) {
				
                foundProduct = baseDados[j];
                
                  if (foundProduct.referencia == ref) {
					  
                      flagFound = 0;

                      write(pipearray[i+1][1], &foundProduct, sizeof(foundProduct));
                      
                      break;
                      
                  }
             }

            if (flagFound == -1) {
				
              notFoundProduct.referencia = ref;
              
              write(pipearray[i+1][1], &notFoundProduct, sizeof(notFoundProduct));
              
            }

            if (4 == i) {

              close(pipearray[0][0]);
              
            }

            close(pipearray[i+1][1]);
        }

    return 0;
}
