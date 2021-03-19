#ifndef __XMOD_H_
#define __XMOD_H_

#include <asm-generic/errno-base.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

#include "util.h"
#include "log.h"
#include "error_handler.h"

char *global_file_path;
int nftot = 0;
int nfmod = 0;
bool father = true;
char *log_dir;
pid_t child;
bool logs = true;

void error_handler();
void signal_handler();
void signal_handler_child();
void signal_handler_hup();
void unlock();
bool prompt();
void write_log(char *event, char *info);

#endif // __XMOD_H_
