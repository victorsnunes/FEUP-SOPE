#ifndef __ERROR_HANDLER_H_
#define __ERROR_HANDLER_H_

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "log.h"

extern bool logs;
void error_handler();

#endif