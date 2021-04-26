#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define NUMBER_OF_IMPUTS 4
#define MAX_RANDOM_NUMBER 1000

bool running = true;

void alarm_handler();
void print_usage();

void *client(void *arg){
    int id = *(int *) arg;
    free(arg);

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
    return NULL;
}

int main(int argc, char** argv){
    int nseconds;
    int public_fifo;
    char *fifopath;

    struct sigaction sigalarm;
    sigset_t smask;

    //PARSER
    if (argc > NUMBER_OF_IMPUTS){
        printf("Too many inputs\n");
        print_usage();
        exit(1);
    } if(argc < NUMBER_OF_IMPUTS){
        printf("Too few inputs\n");
        print_usage();
        exit(1);
    } else {
        for(int i = 1; i < NUMBER_OF_IMPUTS; i++){
            if(argv[i][0] == '-'){
                if(argv[i][1] == 't'){
                    i++;
                    nseconds = atoi(argv[i]);
                } else {
                    printf("Invalid flag\n")
                    print_usage();
                }
            } else {
                fifopath = argv[i];
            }
        }
    }


    //Setup the signal handler for the alarm, and the alarm
    if (sigemptyset(&smask) == -1)
        perror("error on sigemptyset()");

    sigalarm.sa_handler = alarm_handler;
    sigalarm.sa_mask = smask;
    sigalarm.sa_flags = 0;
    if (sigaction(SIGALRM, &sigalarm, NULL) == -1){
        perror("Failed to set up alarm handler");
        exit(1);
    }
    alarm(nseconds);

    // Setup FIFO for Read only
    if ( (public_fifo = open(fifopath, O_WRONLY | O_CREAT)) == -1){
        perror("Failed to open FIFO");
        exit(1);
    }    

    pthread_t thread_id;
    int id = 0;

    while(running){

        int *client_atr = (int *)malloc(sizeof(int));
        *client_atr = id; 

        if(pthread_create(&thread_id, NULL, client, client_atr)){
            perror("Failed to create thread");
        }
    
        if(pthread_detach(thread_id)){
            perror("Failed to detach thread");  
        }

        id++;
        //Random intervals in miliseconds
        sleep((rand()%MAX_RANDOM_NUMBER)/1000);
    }

    if (close(public_fifo) == -1){
        perror("Failed close fifo");
        exit(1);
    }
    printf("nseconds = %d, fifopath = %s\n", nseconds, fifopath);

    return 0;
}

void alarm_handler(){
    running = false;
}

void print_usage(){
    printf("Usage: ./c <-t nsecs> <fifoname>\n");
}