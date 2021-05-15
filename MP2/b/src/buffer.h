#ifndef __BUFFER_H_
#define __BUFFER_H_

#include <stdlib.h>
#include <stdbool.h>

#include "common.h"


void initialize_buffer(int size);
Message *peek();
bool isFull();
bool buffer_is_empty();
int getSize();
bool buffer_insert(Message *msg);
Message *buffer_pop();
void deallocate_buffer();


#endif // __BUFFER_H_
