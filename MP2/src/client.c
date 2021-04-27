#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>

#include <semaphore.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>


#define NUMBER_OF_IMPUTS 4

int client_id = 0;

typedef struct msg { 
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
            }
        }

        //success
        //write log IWANT
        break;
    }

    //espera em bloqueamento pela resposta

    //encerra fifo

    //não esquecer de fazer o registro
}

int main(int argc, char** argv){
    // int nseconds;
    // char *fifopath;

    // //PARSER
    // if (argc > NUMBER_OF_IMPUTS){
    //     printf("Too many inputs, exiting...\n");
    //     exit(1);
    // } if(argc < NUMBER_OF_IMPUTS){
    //     printf("Too few inputs, exiting...\n");
    //     exit(1);
    // } else {
    //     for(int i = 1; i < NUMBER_OF_IMPUTS; i++){
    //         if(argv[i][0] == '-' && argv[i][1] == 't'){
    //             i++;
    //             nseconds = atoi(argv[i]);
    //         } else {
    //             fifopath = argv[i];
    //         }
    //     }
    // }

    // /*while(nseconds){
    //     //lançar clientes por thread
    // }*/

    // printf("nseconds = %d, fifopath = %s\n", nseconds, fifopath);


    // //encerra tudo corretamente

    msg mymessage = create_message()


    return 0;
}
