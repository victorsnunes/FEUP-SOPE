#ifndef __CLIENT_H_
#define __CLIENT_H_

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>

#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "common.h"

#define NUMBER_OF_INPUTS 4
#define MAX_RANDOM_NUMBER 50000
#define BUFFER_SIZE 3000


bool running = true;
pthread_mutex_t lock;
int threads_running = 0;

void alarm_handler();
void print_usage();
void increase_thread_counter();
void decrease_thread_counter();
void invalidate(char *path);
pthread_t threads[BUFFER_SIZE];

//Public Fifo
int public_fifo;

//Main thread tid
pthread_t main_thread_tid;


#endif // __CLIENT_H_
