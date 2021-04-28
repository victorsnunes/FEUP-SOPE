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

typedef struct msg { 
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
} msg;

//Public Fifo
int public_fifo;

//Main thread tid
pthread_t main_thread_tid;

msg create_message(int id, int t, int pid, pthread_t tid, int res)
{
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

int create_private_fifo(){
    
}

int sendMessage(int fifo, msg message){
   // int i = write(public_fifo, )

   return 0;
}


void Client(int id){
    
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
            }else if(errno == EGAIN){ // FULL public FIFO, try again
                usleep(50);
                continue;
            } else { // error sending request
                unlink(private_fifo_path);
                return NULL;
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

        //success
        //write log IWANT
        break;

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

}

int main(int argc, char** argv){

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
