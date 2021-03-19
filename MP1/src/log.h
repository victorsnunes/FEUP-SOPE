#ifndef __LOG_H_
#define __LOG_H_

#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>

#include "error_handler.h"

extern char* log_dir;
extern clock_t start_t;
void write_log(char *event, char *info);

#endif
