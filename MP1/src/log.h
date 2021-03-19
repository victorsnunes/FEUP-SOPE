#ifndef __LOG_H_
#define __LOG_H_

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

#include "error_handler.h"

extern char* log_dir;
void write_log(char *event, char *info);

#endif