#ifndef __SERVER_H_
#define __SERVER_H_

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
#include "lib.h"
#include "buffer.h"

#define NUMBER_OF_INPUTS 6
#define BUFFER_SIZE 128
#define NUMBER_OF_TRIES 5

pthread_t main_thread_tid;
pthread_mutex_t lock;
bool timeout = false;
int threads_running = 0;

void print_usage();
void *consumer();
void *producer(void *arg);
void alarm_handler();
bool send_to_buffer(Message *msg);
Message *extract_from_buffer();
void increase_thread_counter();
void decrease_thread_counter();

#endif // __SERVER_H_
