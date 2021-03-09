#include <asm-generic/errno-base.h>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>


void concatenate(char a[], char b[]);
void error_handler();

int main(int argc, char *argv[]){
  int flag_counter = 0;

  __mode_t mode;
  //char *mode;
  char *file_path;

  char* working_dir = getenv("PWD");
  if(working_dir == NULL){
    printf("Failed to get working directory, exiting...\n");
    exit(-1);
  }

  int i = 1;
  while(i < argc){
    if(argv[i][0] == '-'){
      flag_counter++;
      printf("received flag: %s\n", argv[i]);
    }
    else
      break;
    i++;
  }

  mode = strtol(argv[i], NULL, 8);
  printf("mode = %u\n", mode);
  i++;
  file_path = argv[i];

  concatenate(working_dir, file_path);

  printf("file path = %s\n", working_dir);

  int return_code = chmod(working_dir, mode);

  if(return_code != 0) error_handler();

  return 0;
}

void concatenate(char a[], char b[]){
  int size_a = 0;
  int size_b = 0;

  while(a[size_a] != '\0'){
    size_a++;
  }

  a[size_a] = '/';
  size_a++;

  while(b[size_b] != '\0'){
    a[size_a] = b[size_b];
    size_a++;
    size_b++;
  }
  a[size_a] = '\0';

}

void error_handler(){
  switch (errno) {
    case EACCES:
      printf("Permission denied\n");
      break;
    case EFAULT:
      printf("Pathname no accessible\n");
      break;
    case EIO:
      printf("An I/O error occured\n");
      break;
    case ELOOP:
      printf("Too many symbolic links\n");
      break;
    case ENOENT:
      printf("File not found\n");
      break;
    case ENOTDIR:
      printf("Not a directory\n");
      break;
    default:
      printf("something bad happend\n");
      break;
  }
  exit(-1);
}
