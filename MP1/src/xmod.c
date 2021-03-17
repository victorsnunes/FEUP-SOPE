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

#define UNKNOWN_FLAG 21

char* file_path;
int nftot = 0;
int nfmod = 0;

void concatenate(char a[], char b[]);
void error_handler();
void error_unknow_flag(char flag);
void signal_handler(int signo);
bool prompt();

int main(int argc, char *argv[]){
  bool first_agr = true;

  //flags
  //TODO implement Verbose
  bool verbose = false;
  bool verboseC = false;
  bool recursive = false;
  bool logs = true;

  __mode_t mode;
  char* working_dir = getenv("PWD");
  char* log_dir = getenv("LOG_FILENAME");
  int return_code = 0;
  int dir_i;

  struct dirent *entry;
  DIR *dir;

  struct sigaction new;
  sigset_t smask;

  pid_t child;

  if(log_dir == NULL) logs = false;

  if(logs) printf("Logs activated!\n");

  if(working_dir == NULL){
    printf("Failed to get working directory, exiting...\n");
    exit(-1);
  }

  if(sigemptyset(&smask) == -1)
    perror("error on sigemptyset()");
  new.sa_handler = signal_handler;
  new.sa_mask = smask;
  new.sa_flags = 0;

  if(sigaction(SIGINT, &new, NULL) == -1)
    perror("error on sigaction()");

  nftot++; //Not sure where to put this

  //random code so compiler stop complaining
  if(verbose && verboseC) verbose = verboseC;

  //TODO accept absolute paths
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
        first_agr = false;
      } else {
        file_path = argv[i];
        dir_i = i;
      }
    }
  }

  concatenate(working_dir, file_path);

  //TODO error handler for opendir
  if(recursive){
    if((dir = opendir(working_dir)) == NULL) {
      error_handler();
    } else {
      while((entry = readdir(dir)) != NULL){
        char *home_path_r = working_dir;
        printf("pwd = %s\n", home_path_r);

        if(entry->d_name[0] != '.'){
          char* file_path_r = entry->d_name;
          //concatenate(home_path_r, file_path_r);
          printf("forking for dir = %s, with home = %s\n", file_path_r, home_path_r);
          child = fork();
          if(child == 0){
            argv[dir_i] = file_path_r;
            main(argc, argv);
          }
           //else kill(child, SIGINT);
        }
      }
    }
  }

  sleep(5);

  printf("chmod on file: %s\n", working_dir);
  return_code = chmod(working_dir, mode);
  nfmod++;
  if(return_code != 0) error_handler();

  //TODO error handler for child
  if(child != 0){
    pid_t r = wait(NULL);
    printf("child with pid = %d has finished\n", r);
  }


  return 0;
}

//Concatenate in the format home/file and return it through file
void concatenate(char home[], char file[]){
  int size_a = 0;
  int size_b = 0;

  char* tmp = home;

  while(tmp[size_a] != '\0'){
    size_a++;
  }

  if(tmp[size_a - 1] != '/') {
    tmp[size_a] = '/';
    size_a++;
  }

  while(file[size_b] != '\0'){
    tmp[size_a] = file[size_b];
    size_a++;
    size_b++;
  }
  tmp[size_a] = '\0';

  file = tmp;

}

void signal_handler(int signo){
  printf("Received Signal num: %d\n", signo);
  printf("%d ; %s ; %d ; %d\n", getpid(), file_path, nftot, nfmod);
  if(prompt()) return;
  exit(-1);
}

bool prompt(){
  char response[10];
  while(1){
    printf("Are you sure you want to cancel? (y/n)\n");
    scanf("%s", response);
    if(response[0] == 'y' || response[0] == 'Y') return true;
    else if(response[0] == 'n' || response[0] == 'N') return false;
  }
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
