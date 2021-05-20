#include "server.h"

int main(int argc, char **argv){
    main_thread_tid = pthread_self();

    int nseconds;
    int bufsz = 10;

    //Interruptions
    struct sigaction sigalarm;
    sigset_t smask;

    //Turn of the stdout buffer
    setvbuf(stdout, NULL, _IONBF, 0);

    //Parse the command line
    if (argc > NUMBER_OF_INPUTS){
        printf("Too many inputs %d\n", argc);
        print_usage();
        exit(1);
    } else if(argc < NUMBER_OF_INPUTS - 2){
        printf("Too few inputs\n");
        print_usage();
        exit(1);
    } else {
        for(int i = 1; i < argc; i++){
            if(argv[i][0] == '-'){
                if(argv[i][1] == 't'){
                    i++;
                    nseconds = atoi(argv[i]);
                }
                else if(argv[i][1] == 'l'){
                    i++;
                    bufsz = atoi(argv[i]);
                } else {
                    printf("Invalid flag '%c'\n", argv[i][1]);
                    print_usage();
                    exit(1);
                }
            } else {
                public_fifopath = argv[i];
            }
        }
    }

    //Setup the signal handler for the alarm
    if(sigemptyset(&smask)){
        perror("[server] Error on sigemptyset()");
    }
    sigalarm.sa_handler = alarm_handler;
    sigalarm.sa_mask = smask;
    sigalarm.sa_flags = 0;
    if(sigaction(SIGALRM, &sigalarm, NULL) == -1){
        perror("[server] Failed to setup the alarm handler");
        return 1;
    }


    //Seting up the alarm
    alarm(nseconds);

    //Open and create public fifo
    if(mkfifo(public_fifopath, 0666)){
        perror("[server] Failed to create public fifo");
        exit(1);
    }

    int public_fifo = open(public_fifopath, O_RDONLY);
    if(public_fifo == -1){
        perror("[server] Failed to open public fifo");
        return 1;
    }

    //Create the consumer thread
    pthread_t consumer_tid;
    if(pthread_create(&consumer_tid, NULL, consumer, NULL)){
        perror("[server] Failed to create consumer thread");
        return 1;
    }

    //Initialize the Mutex
    if(pthread_mutex_init(&bufferLock, NULL) != 0){
        perror("[server] Failed to init buffer Mutex");
        exit(1);
    }
    if(pthread_mutex_init(&counterLock, NULL) != 0){
        perror("[server] Failed to init counter Mutex");
        exit(1);
    }

    //Initialize the buffer
    initialize_buffer(bufsz);

    //Main loop where we get the request and create threads
    while(!timeout){
        Message *request = malloc(sizeof(Message));
        int ret;
        pthread_t producer_tid;

        ret = read(public_fifo, request, sizeof(Message));
        if(ret == -1 || ret == 0){
            //failed caused by interruption
            if(timeout){
                printf("Finished execution\n");
                unlink(public_fifopath);
                break;
            }
            //normal error
            else{
                unlink(public_fifopath);
                perror("[server] Failed to read from public fifo");
                break;
            }

        }

        //There is no more time, sending the request direct to the consumer
        if(timeout){
            printf("%ld ; %d ; %d ; %d ; %lu ; %d ; %s\n", time(NULL), request->rid, request->tskload, request->pid, request->tid, request->tskres, "2LATE");
            send_to_buffer(request);
        }
        else{
            printf("%ld ; %d ; %d ; %d ; %lu ; %d ; %s\n", time(NULL), request->rid, request->tskload, request->pid, request->tid, request->tskres, "RECVD");

            while(pthread_create(&producer_tid, NULL, producer, request)){
                //Too many threads, wait and try again
                if(errno == EAGAIN){
                    usleep(50);
                    continue;
                }
                perror("[server] Failed to create producer thread");
                free(request);
                break;
            }
        }

    }

    //Wait for all the threads to terminate
    while(threads_running > 0);

    deallocate_buffer();

    printf("[server] Finished execution\n");

    return 0;
}


void *consumer(){
    increase_thread_counter();

    struct sigaction sigpipe;
    sigset_t smask;

    //Setup the signal handler for the failed to write pipe
    if(sigemptyset(&smask)){
        perror("[server] Error on sigemptyset()");
    }
    sigpipe.sa_handler = sigpipe_handler;
    sigpipe.sa_mask = smask;
    sigpipe.sa_flags = 0;
    if(sigaction(SIGPIPE, &sigpipe, NULL) == -1){
        perror("[server] Failed to setup the alarm handler");
        return NULL;
    }

    //Loop while there is time and while the buffer is not empty and there still producer threads running
    while(!timeout || !buffer_is_empty() || threads_running > 1){
        if(!buffer_is_empty()){
            Message *response = extract_from_buffer();
            char private_fifopath[BUFFER_SIZE];
            sprintf(private_fifopath, "/tmp/%d.%ld", response->pid, response->tid);

            //Open the private fifo and deals with errors
            int private_fifo = open(private_fifopath, O_WRONLY);
            int tries = 0;
            while(private_fifo == -1){
                //Too many open files, wait and try again
                if(errno == EMFILE || errno == ENOMEM){
                    tries++;
                    if(tries == NUMBER_OF_TRIES){
                        printf("%ld ; %d ; %d ; %d ; %lu ; %d ; %s\n", time(NULL), response->rid, response->tskload, response->pid, response->tid, response->tskres, "FAILD");
                        fprintf(stderr, "[server] Failed to open private fifo, too many tries giving up\n");
                        goto next;
                    }
                    usleep(50);
                    continue;
                }
                if(errno == ENOENT){
                    fprintf(stderr, "Failed to open private fifo file not found\n");
                    printf("%ld ; %d ; %d ; %d ; %lu ; %d ; %s\n", time(NULL), response->rid, response->tskload, response->pid, response->tid, response->tskres, "FAILD");
                    goto next;
                }
                else{
                    perror("[server] Failed to open private fifo");
                    printf("%ld ; %d ; %d ; %d ; %lu ; %d ; %s\n", time(NULL), response->rid, response->tskload, response->pid, response->tid, response->tskres,"FAILD");
                    goto next;
                }
            }

            //Send the response
            if(write(private_fifo, response, sizeof(*response)) == -1){
                perror("[server] Failed to send response");
                printf("%ld ; %d ; %d ; %d ; %lu ; %d ; %s\n", time(NULL), response->rid, response->tskload, response->pid, response->tid, response->tskres, "FAILD");
                goto next;
            }

            if(response->tskres == -1)
                printf("%ld ; %d ; %d ; %d ; %lu ; %d ; %s\n", time(NULL), response->rid, response->tskload, response->pid, response->tid, response->tskres, "2LATE");
            else
                printf("%ld ; %d ; %d ; %d ; %lu ; %d ; %s\n", time(NULL), response->rid, response->tskload, response->pid, response->tid, response->tskres, "TSKDN");

        next:
            free(response);
        }
        //buffer is clear, wait and try again
        usleep(50);
    }
    decrease_thread_counter();
    return NULL;
}

void *producer(void *arg){

    Message *request = (Message *) arg;

    increase_thread_counter();

    request->tskres = task(request->tskload);

    printf("%ld ; %d ; %d ; %d ; %lu ; %d ; %s\n", time(NULL), request->rid, request->tskload, request->pid, request->tid, request->tskres, "TSKEX");

   while(!send_to_buffer(request)){
        //Buffer full wait and try again
        usleep(20);
    }

    decrease_thread_counter();
    return NULL;
}

void alarm_handler(){
    timeout = true;
    unlink(public_fifopath);
}

void sigpipe_handler(){
    return;
}

bool send_to_buffer(Message *msg){
    bool ret;
    pthread_mutex_lock(&bufferLock);
    ret = buffer_insert(msg);
    pthread_mutex_unlock(&bufferLock);
    return ret;
}

Message *extract_from_buffer(){
    Message *ret;
    pthread_mutex_lock(&bufferLock);
    ret = buffer_pop();
    pthread_mutex_unlock(&bufferLock);
    return ret;
}

void increase_thread_counter(){
    pthread_mutex_lock(&counterLock);
    threads_running++;
    pthread_mutex_unlock(&counterLock);
}

void decrease_thread_counter(){
    pthread_mutex_lock(&counterLock);
    threads_running--;
    pthread_mutex_unlock(&counterLock);
}

void print_usage(){
    printf("Usage: ./s <-t nsecs> [-l bufsz] <fifoname>\n");
}
