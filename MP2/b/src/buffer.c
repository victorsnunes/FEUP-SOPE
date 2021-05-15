#include "buffer.h"

Message **buffer;
int front = 0;
int rear = -1;
int counter = 0;
int max_size = -1;

void initialize_buffer(int size){
    max_size = size;
    buffer = (Message **) malloc(sizeof(Message *) * size);
}

Message* peek(){
    return buffer[front];
}

bool isFull(){
    return counter == max_size;
}

bool buffer_is_empty(){
    return counter == 0;
}

int getSize(){
    return counter;
}

bool buffer_insert(Message *msg){

    if(!isFull()){

        if(rear == max_size - 1){
            rear = -1;
        }

        buffer[++rear] = msg;
        counter++;
        return true;
    }
    return false;
}

Message *buffer_pop(){
    Message *ret = buffer[front++];

    if (front == max_size){
        front = 0;
    }
    counter--;
    return ret;
}

void deallocate_buffer(){
    free(buffer);
}
