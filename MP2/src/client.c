#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>

#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "common.h"

#define NUMBER_OF_INPUTS 4
#define MAX_RANDOM_NUMBER 1000
#define BUFFER_SIZE 3000


bool running = true;
pthread_mutex_t lock;
int threads_running = 0;

void alarm_handler();
void print_usage();
void increase_thread_counter();
void decrease_thread_counter();

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
    public_fifo = open(fifo_path, O_WRONLY | O_NONBLOCK);

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

    increase_thread_counter();

    //Create private fifo
    char private_fifo_path[BUFFER_SIZE];
    sprintf(private_fifo_path, "/tmp/%d.%ld", pid, tid);

    if(mkfifo(private_fifo_path, 0660)){
        perror("error log private");
        printf("Error creating private FIFO\n");
        decrease_thread_counter();
        return NULL;
    }

    //Create message
    Message message = create_message(id, t, pid, tid);
    int private_fifo = 0;

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
                printf("%ld ; %d ; %d ; %d ; %lu ; %d ; %s\n", time(NULL), id, t, pid, tid, -1, "CLOSD");
                unlink(private_fifo_path);
                decrease_thread_counter();
                return NULL;
            } else if(errno == EAGAIN){ // FULL public FIFO, try again
                usleep(50);
                continue;
            } else { // error sending request
                unlink(private_fifo_path);
                decrease_thread_counter();
                return NULL;
            }
        }

        break;
    }

    //Client just made a request
    printf("%ld ; %d ; %d ; %d ; %lu ; %d ; %s\n", time(NULL), id, t, pid, tid, -1, "IWANT");

    Message response;
    while((private_fifo = open(private_fifo_path, O_RDONLY)) == -1){
        //Too many open fd, wait and try again
        if(errno == EMFILE || errno == ENOMEM){
            usleep(50);
            continue;
        }
        //error caused by alarm
        if(!running){
            unlink(private_fifo_path);
            decrease_thread_counter();
            printf("%ld ; %d ; %d ; %d ; %lu ; %d ; %s\n", time(NULL), id, t, pid, tid, -1, "GAVUP");
            return NULL;
        }
        //Somthing unexpected happend
        else{
            unlink(private_fifo_path);
            decrease_thread_counter();
            perror("Failed to open private fifo");
            return NULL;
        }

    }

    read(private_fifo, &response, sizeof(response));

    //Received message
    printf("%ld ; %d ; %d ; %d ; %lu ; %d ; %s\n", time(NULL), id, t, pid, tid, response.tskres, "GOTRS");
    
    close(private_fifo);
    unlink(private_fifo_path);
    decrease_thread_counter();
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

    //Setup Mutex
    if(pthread_mutex_init(&lock, NULL) != 0){
        perror("Failed to init Mutex");
        exit(1);
    }

    pthread_t thread_id;
    int id = 0;

    while(running){

        int *client_atr = (int *)malloc(sizeof(int));
        *client_atr = id; 

        while(pthread_create(&thread_id, NULL, client, client_atr)){
            //Too many threads, wait and try again
            if(errno == EAGAIN){
                usleep(50);
                continue;
            }
            perror("Failed to create thread");
            break;
        }
    
        if(pthread_detach(thread_id)){
            perror("Failed to detach thread");
            break;
        }

        id++;

        //Random intervals in miliseconds
        usleep((rand()%MAX_RANDOM_NUMBER));
    }

    if (close(public_fifo) == -1){
        perror("Failed close fifo");
        exit(1);
    }

    printf("Finished to spit threads\n");

    //Wait to all threads finish
    while(threads_running > 0);
    pthread_mutex_destroy(&lock);

    return 0;
}

void increase_thread_counter(){
    pthread_mutex_lock(&lock);
    threads_running++;
    pthread_mutex_unlock(&lock);
}

void decrease_thread_counter(){
    pthread_mutex_lock(&lock);
    threads_running--;
    pthread_mutex_unlock(&lock);
}

void alarm_handler(){
    running = false;
}

void print_usage(){
    printf("Usage: ./c <-t nsecs> <fifoname>\n");
}
