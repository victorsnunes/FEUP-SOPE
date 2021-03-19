#ifndef __PERMISSIONS_H_
#define __PERMISSIONS_H_

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
#include <ctype.h>
#include <string.h>


// State machine to parse normal mode args
void mode_state_machine(char *argv);

//Get file initial permissions
int getPermissions(const char *path);

//Caculate file final permissions (octal)
int changePermissions(int oldPerms);

#endif
