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

#define NUMBER_OF_INPUTS 4
#define MAX_RANDOM_NUMBER 1000
#define BUFFER_SIZE 3000

bool running = true;

typedef struct{
    int id;
    int t;
    pid_t pid;
    pthread_t tid;
    int res;
} msg; 

void alarm_handler();
void print_usage();

//Public Fifo
int public_fifo;

int alarm_stat = 0;

//Main thread tid
pthread_t main_thread_tid;

msg create_message(int id, int t, int pid, pthread_t tid){
    int res = -1;
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



int sendMessage(int fifo, msg message){
   // int i = write(public_fifo, )

   return 0;
}




void *Client(void *arg){
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
    msg message = create_message(id, t, pid, tid);
    msg message2;
    int private_fifo = 0;

    //Sends a request in the public fifo
    while(1){
        int ret;
        ret = write(public_fifo, &message, sizeof(message));

        //error 
        if(ret == -1){
            if(errno == EPIPE){ // server closes public FIFO
                if(alarm_stat == 0){
                    alarm(0);
                    pthread_kill(main_thread_tid, SIGALRM);
                }

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

      while ((private_fifo = open (private_fifo_path, O_RDONLY)) < 0){
        read(private_fifo, &message2, sizeof(message2));
        printf("mensagem recebida %d\n", message2.res);
    }
    //TODO: Esperar em bloqueamento pela resposta
    //int ret2 = read(private_fifo, &message2, sizeof(message2));
   // printf("message recieved: %d", message2.res);


    //TODO: Fechar fifos, acabou o tempo estipulado pelo utilizador

    //TODO: não esquecer de fazer o registro
    return NULL;
}

int main(int argc, char** argv){
    main_thread_tid = pthread_self();
    srand(time(NULL));

    int nseconds;
    char *fifopath;

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
    if ( open_public_fifo(fifopath) == 1){
        exit(1);
    }    

    pthread_t thread_id;
    int id = 0;

    while(running){

        int *client_atr = (int *)malloc(sizeof(int));
        *client_atr = id; 

        if(pthread_create(&thread_id, NULL, Client, client_atr)){
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

    //TODO: Fazer registros

    return 0;
}

void alarm_handler(){
    running = false;
}

void print_usage(){
    printf("Usage: ./c <-t nsecs> <fifoname>\n");
}
