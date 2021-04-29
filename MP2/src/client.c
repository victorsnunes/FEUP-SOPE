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

/*  TEMP   */
#define ALARM_CHILL 0
int alarm_stat = 0;
void open_private_fifo() {};


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

//Main thread tid
pthread_t main_thread_tid;

msg create_message(int id, int t, pid_t pid, pthread_t tid){
    int res = -1;
    msg message = {id, t, pid, tid, res};

    return message;
}

int open_public_fifo(char* fifo_path){
    //TODO: Remove O_CREAT, server é responsavel por criar o fifo publico
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

int sendMessage(int fifo, msg message){
   // int i = write(public_fifo, )

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
        perror("Erro creating private FIFO");
        return NULL;
    }
    printf("Created private fifo\n");

    //Create message
    msg message = create_message(id, t, pid, tid);

    //Sends a request in the public fifo
    while(1){
        int ret;
        ret = write(public_fifo, &message, sizeof(message));

        //error
        if(ret == -1){
            if(errno == EPIPE){ // server closes public FIFO
                if(running){
                    alarm(0);
                    pthread_kill(main_thread_tid, SIGALRM);
                }
                unlink(private_fifo_path);
                return NULL;
            } else if(errno == EAGAIN){ // public FIFO FULL, try again
                usleep(50);
                continue;
            } else { // error sending request
                unlink(private_fifo_path);
                return NULL;
            }
        }

        break;
    }
    printf("Message sent msg = {%d, %d, %d, %lu }\n", id, t, pid, tid);

    int private_fifo;
    private_fifo = open(private_fifo_path, O_RDONLY);
    printf("private fifo = %d\n", private_fifo);
    if (private_fifo == -1){
        printf("Failed to open private fifo\n");
        perror("Failed open private fifo");
        unlink(private_fifo_path);
        return NULL;
    }

    printf("private FIFO open\n");

    msg response;
    read(private_fifo, &response, sizeof(response));
    /*
    while(1){
        int ret = read(private_fifo, &response, sizeof(response));
        printf("response = %d\n", ret);
        if(ret == -1){
            if(errno == EAGAIN || errno == EWOULDBLOCK){
                usleep(50);
                continue;
            }
            else {
                printf("error dfaihs çlkasdfçlkashedfç");
                perror("Failed to read from private FIFO");
                unlink(private_fifo_path);
                pthread_exit(NULL);
            }
        }
        if (ret == 0){
            usleep(50);
            continue;
        }

        break;
    }
    */
    printf("message recieved: %d", response.res);


    if(close(private_fifo)){
        perror("Failed to close Private FIFO");
        unlink(private_fifo_path);
        return NULL;
    }
    unlink(private_fifo_path);

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
    if ( open_public_fifo(fifopath) == 1){
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
    
        /*if(pthread_detach(thread_id)){
            perror("Failed to detach thread");
            break;
        }*/


        id++;
        running = false;
        //Random intervals in miliseconds
        usleep((rand()%MAX_RANDOM_NUMBER));
    }

    if (close(public_fifo) == -1){
        perror("Failed close fifo");
        exit(1);
    }

    pthread_join(thread_id, NULL);
    /*TEMP*/
    //unlink(fifopath);

    //TODO: Fazer registros

    return 0;
}

void alarm_handler(){
    running = false;
}

void print_usage(){
    printf("Usage: ./c <-t nsecs> <fifoname>\n");
}
