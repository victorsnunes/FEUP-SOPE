#ifndef __XMOD_H_
#define __XMOD_H_

#include <asm-generic/errno-base.h>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>

#include "util.h"

char* global_file_path;
int nftot = 0;
int nfmod = 0;
bool father = true;

pid_t child;

void error_handler();
void error_unknow_flag(char flag);
void signal_handler(int signo);
void signal_handler_child(int signo);
void unlock();
bool prompt();


#endif // __XMOD_H_
