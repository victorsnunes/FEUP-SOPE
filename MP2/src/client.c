#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>


#define NUMBER_OF_IMPUTS 4


void Client(int id){
    struct msg { 
        int i;
        int t;
        pid_t pid;
        pthread_t tid;
        int res;
    };

    //abre o fifo privado

    //envia pedido ao servidor

    //espera em bloqueamento pela resposta

    //encerra fifo

    //não esquecer de fazer o registro
}

int main(int argc, char** argv){
    int nseconds;
    char *fifopath;

    //PARSER
    if (argc > NUMBER_OF_IMPUTS){
        printf("Too many inputs, exiting...\n");
        exit(1);
    } if(argc < NUMBER_OF_IMPUTS){
        printf("Too few inputs, exiting...\n");
        exit(1);
    } else {
        for(int i = 1; i < NUMBER_OF_IMPUTS; i++){
            if(argv[i][0] == '-' && argv[i][1] == 't'){
                i++;
                nseconds = atoi(argv[i]);
            } else {
                fifopath = argv[i];
            }
        }
    }

    while(nseconds){
        //lançar clientes por thread
    }

    printf("nseconds = %d, fifopath = %s\n", nseconds, fifopath);


    //encerra tudo corretamente

    return 0;
}
