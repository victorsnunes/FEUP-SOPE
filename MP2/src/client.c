#include <string.h>
#include <pthread.h>
#include <errno.h>

#include "common.h"

#include <semaphore.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
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

#define NUMBER_OF_INPUTS 4
#define MAX_RANDOM_NUMBER 1000
#define BUFFER_SIZE 3000

/*  TEMP   */
#define ALARM_CHILL 0
int alarm_stat = 0;
void open_private_fifo() {};


bool running = true;

void alarm_handler();
void print_usage();

//Public Fifo
int public_fifo;

//Main thread tid
pthread_t main_thread_tid;

Message create_message(int id, int t, int pid, pthread_t tid){
    int res = -1;
    Message message = {id, pid, tid, t, res};

    return message;
}

int open_public_fifo(char* fifo_path){
    public_fifo = open(fifo_path, O_WRONLY | O_NONBLOCK | O_CREAT);

    if(public_fifo == -1){
        perror("Error opening public FIFO");
        return 1;
    }

    return 0;
}

int create_private_fifo(pid_t pid, pthread_t tid){
    char private_fifo_path[BUFFER_SIZE];
    sprintf(private_fifo_path, "/tmp/%d.%lu", pid, tid);

    if(mkfifo(private_fifo_path, 0660)){
        printf("Error creating private FIFO\n");
        return 1;
    }
    return 0;
}


void *client(void *arg){
    int id = *(int *) arg;
    free(arg);

    pid_t pid = getpid();
    pthread_t tid = pthread_self();
    int t = rand()%8 + 1;

    //Create private fifo
    char private_fifo_path[BUFFER_SIZE];
    sprintf(private_fifo_path, "/tmp/%d.%ld", pid, tid);

    if(mkfifo(private_fifo_path, 0660)){
        perror("error log private");
        printf("Error creating private FIFO\n");
        return NULL;
    }
  
 
    // //TODO: Criar função
    // int private_fifo;
    // if((private_fifo = open(private_fifo_path, O_RDONLY)) == -1){
    //     printf("error opening private fifo\n");
    //     unlink(private_fifo_path);
    //     return NULL;
    // }

    //Create message
    Message message = create_message(id, t, pid, tid);
    int private_fifo = 0;

    //Sends a request in the public fifo
    while(1){
        int ret;
        ret = write(public_fifo, &message, sizeof(message));

        //Client just made a request
        printf("%d ; %d ; %d ; %d ; %d ; %d ; %s", time(), id, t, pid, tid, -1, "IWANT");

        //error 
        if(ret == -1){
            if(errno == EPIPE){ // server closes public FIFO
                if(alarm_stat == 0){
                    alarm(0);
                    pthread_kill(main_thread_tid, SIGALRM);
                }
                //TODO: Deve esperar resposta...
                unlink(private_fifo_path);
                return NULL;
            } else if(errno == EAGAIN){ // FULL public FIFO, try again
                usleep(50);
                continue;
            } else { // error sending request
                unlink(private_fifo_path);
                return NULL;
            }
        }

        break;
    }

    Message response;
    private_fifo = open (private_fifo_path, O_RDONLY);

    read(private_fifo, &response, sizeof(response));
    printf("mensagem recebida %d\n", response.tskres);

    //TODO: Esperar em bloqueamento pela resposta
    //int ret2 = read(private_fifo, &message2, sizeof(message2));
   // printf("message recieved: %d", message2.res);


    //TODO: Fechar fifos, acabou o tempo estipulado pelo utilizador

    //TODO: não esquecer de fazer o registro
    //char oper[];
    //printf("%d ; %d ; %d ; %d ; %d ; %d ; %s", time(), id, t, pid, tid, ret, oper);

    return NULL;
}

int main(int argc, char** argv){
    main_thread_tid = pthread_self();
    srand(time(NULL));

    int nseconds;
    char *fifopath;

    struct sigaction sigalarm;
    sigset_t smask;

    //PARSER
    if (argc > NUMBER_OF_INPUTS){
        printf("Too many inputs\n");
        print_usage();
        exit(1);
    } if(argc < NUMBER_OF_INPUTS){
        printf("Too few inputs\n");
        print_usage();
        exit(1);
    } else {
        for(int i = 1; i < NUMBER_OF_INPUTS; i++){
            if(argv[i][0] == '-'){
                if(argv[i][1] == 't'){
                    i++;
                    nseconds = atoi(argv[i]);
                } else {
                    printf("Invalid flag\n");
                    print_usage();
                }
            } else {
                fifopath = argv[i];
            }
        }
    }

    // Setup FIFO for Read only
    if (open_public_fifo(fifopath) == 1){
        exit(1);
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

    pthread_t thread_id;
    int id = 0;

    while(running){

        int *client_atr = (int *)malloc(sizeof(int));
        *client_atr = id; 

        if(pthread_create(&thread_id, NULL, client, client_atr)){
            perror("Failed to create thread");
            break;
        }
    
        if(pthread_detach(thread_id)){
            perror("Failed to detach thread");
            break;
        }

        id++;

        running = false;
        //Random intervals in miliseconds
        usleep((rand()%MAX_RANDOM_NUMBER));
    }

    if (close(public_fifo) == -1){
        perror("Failed close fifo");
        exit(1);
    }

    //TODO: Fazer registros

    return 0;
}

void alarm_handler(){
    running = false;
}

void print_usage(){
    printf("Usage: ./c <-t nsecs> <fifoname>\n");
}
