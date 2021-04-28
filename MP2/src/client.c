#include <string.h>
#include <pthread.h>
#include <errno.h>

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

#define NUMBER_OF_IMPUTS 4
#define MAX_RANDOM_NUMBER 1000

bool running = true;

int client_id = 0;

typedef struct{
    int id;
    int t;
    pid_t tid;
    pthread_t tid;
    int res;
} msg; 

void alarm_handler();
void print_usage();

//Public Fifo
int public_fifo;

//Main thread tid
pthread_t main_thread_tid;

msg create_message(int id, int t, int pid, pthread_t tid, int res){
    msg message = {id, t, pid, tid, res};

    return message;
}

int open_public_fifo(char* fifo_path){
    public_fifo = open(fifo_path, O_WRONLY | O_NONBLOCK);

    if(public_fifo == -1){
        printf("Error opening public FIFO");
        return 1;
    }

    return 0;
}

int create_private_fifo(){
    char private_fifo_path[3000];
   // create path sprintf()

    if(mkfifo(private_fifo_path, 0660)){
        printf("Error creating private FIFO");
        return 1;
    }

    return 0;
}

int sendMessage(int fifo, msg message){
   // int i = write(public_fifo, )

   return 0;
}


void *Client(void *arg){
    int id = *(int *) arg;
    free(arg);

    //abre o fifo privado
    open_private_fifo();

    //Criar mensagem
    create_message(client_id, 1, getpid(), pthread_self());

    //envia pedido ao servidor
    while(1){
        int ret;
        ret = write(public_fifo, message, sizeof(message));

        //error 
        if(ret == -1){
            if(errno == EPIPE){ // server closes public FIFO
                if(alarm_stat == ALARM_CHILL){
                    alarm(0);
                    pthread_kill(main_thread_tid, SIGALRM);
                }

                unlink(private_fifo_path);
                return NULL;
            } else if(errno == EGAIN){ // FULL public FIFO, try again
                usleep(50);
                continue;
            } else { // error sending request
                unlink(private_fifo_path);
                return NULL;
    //não esquecer de fazer o registro
    return NULL;
}

int main(int argc, char** argv){
    main_thread_tid = pthread_self();

    // Setup FIFO for Read only
    if ( open_public_fifo() == 1){
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
