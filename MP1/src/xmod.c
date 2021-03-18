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

char* global_file_path;
int nftot = 0;
int nfmod = 0;


void concatenate(char a[], char b[]);
void error_handler();
void error_unknow_flag(char flag);
void signal_handler(int signo);
void copy(char a[], char b[]);
bool prompt();

int main(int argc, char *argv[]){
  bool first_agr = true;

  //flags
  //TODO implement Verbose
  bool verbose = false;
  bool verboseC = false;
  bool recursive = false;
  bool logs = true;
  bool absolute_path = false;

  __mode_t mode;
  char file_path[256];
  char working_dir[256];
  copy(getenv("PWD"), working_dir);
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

  //FIXME check if getenv is working
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
        if(argv[i][0] == '/') absolute_path = true;
        copy(argv[i], file_path);
        dir_i = i;
      }
    }
  }

  if(!absolute_path)concatenate(working_dir, file_path);
  else copy(file_path, working_dir);

  printf("working dir = %s\n", working_dir);

  //TODO error handler for opendir
  if(recursive){
    if((dir = opendir(working_dir)) == NULL) {
      perror("Failed to open dir...");
      //error_handler();
    } else {
      while((entry = readdir(dir)) != NULL){
        if(entry->d_name[0] == '.') continue;
        char folder_r[256];
        copy(working_dir, folder_r);
        concatenate(folder_r, entry->d_name);


        if( (child = fork()) == -1)
          perror("Failed to fork()");
        if(child == 0){
          argv[dir_i] = folder_r;
          printf("Calling main with argv = %s\n", argv[dir_i]);
          main(argc, argv);
          break;
        }

      }
      closedir(dir);
    }
  }
  //sleep(5);

  global_file_path = working_dir;
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

  while(home[size_a] != '\0'){
    size_a++;
  }

  if(home[size_a - 1] != '/') {
    home[size_a] = '/';
    size_a++;
  }

  while(file[size_b] != '\0'){
    home[size_a] = file[size_b];
    size_a++;
    size_b++;
  }
  home[size_a] = '\0';
}

//Copy what is inside a to b
void copy(char a[], char b[]){
  int i = 0;
  for(; a[i] != '\0'; i++){
    b[i] = a[i];
  }
  b[i] = '\0';
}

void signal_handler(int signo){
  printf("Received Signal num: %d\n", signo);
  printf("%d ; %s ; %d ; %d\n", getpid(), global_file_path, nftot, nfmod);
  if(!prompt()){
    printf("returning...");
    return;
  }
  exit(-1);
}

bool prompt(){
  char response[10];
  while(1){
    setbuf(stdout, NULL);
    printf("Are you sure you want to cancel? (y/n)\n");
    scanf("%s", response);
    printf("response %c\n", response[0]);
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
