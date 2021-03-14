#include <asm-generic/errno-base.h>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>

#define UNKNOWN_FLAG 21

void concatenate(char a[], char b[]);
void error_handler();
void error_unknow_flag(char flag);

int main(int argc, char *argv[]){
  bool first_agr = true;

  //flags
  bool verbose = false;
  bool verboseC = false;
  bool recursive = false;
  bool logs = true;

  __mode_t mode;
  char* file_path;
  char* working_dir = getenv("PWD");
  char* log_dir = getenv("LOG_FILENAME");
  int return_code = 0;

  struct dirent *entry;
  DIR *dir;

  if(log_dir == NULL) logs = false;

  if(logs) printf("Logs activated!\n");

  if(working_dir == NULL){
    printf("Failed to get working directory, exiting...\n");
    exit(-1);
  }

  //Parse the arguments lines
  for(int i = 1; i < argc; i++){
    if(argv[i][0] == '-'){
      switch(argv[i][1]){
        case 'v':
          verbose = true;
          break;
        case 'c':
          verboseC = true;
          break;
        case 'r':
          recursive = true;
          break;
        default:
          error_unknow_flag(argv[i][1]);
      }
    } else {
      if(first_agr) {
        mode = strtol(argv[i], NULL, 8);
        printf("mode = %u\n", mode);
        first_agr = false;
      } else {
        file_path = argv[i];
      }
    }
  }


  if(verbose) printf("Verbose mode activated!\n");
  if(verboseC) printf("Verbose C mode activated!\n");
  if(recursive) printf("Recursive mode activated!\n");


  concatenate(working_dir, file_path);

  //Testing opendir
  if((dir = opendir(working_dir)) == NULL) {
      printf("opendir() error");
  } else {
    while((entry = readdir(dir)) != NULL){
      if(entry->d_name[0] != '.')
        printf("dir = %s\n", entry->d_name);
    }

  }

  printf("file path = %s\n", working_dir);

  return_code = chmod(working_dir, mode);

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

void error_unknow_flag(char flag){
      printf("Error: Unknow flag \'%c\'\n", flag);
      exit(-1);
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
