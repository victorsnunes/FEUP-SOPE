#include "xmod.h"

int main(int argc, char *argv[])
{
  bool first_agr = true;

  struct stat *stat_buffer;

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
  char *log_dir = getenv("LOG_FILENAME");
  int return_code = 0;
  int dir_i;

  struct dirent *entry;
  DIR *dir;

  struct sigaction sigint, sigusr, sighup;
  sigset_t smask;

  //pid_t child;

  if (log_dir == NULL)
    logs = false;

  //Write log for process created
  if (logs)
  {
    char commandline_args[1024] = argv[0];

    for (int i = 1; i < argc; i++)
    {
      strcat(commandline_args, " : ");
      strcat(commandline_args, argv[i]);
    }

    write_log("PROC_CREAT", commandline_args);
  }

  //FIXME check if getenv is working
  if (working_dir == NULL)
  {
    printf("Failed to get working directory, exiting...\n");

    if (logs)
      write_log("PROC_EXIT", "-1");

    exit(-1);
  }

  if (sigemptyset(&smask) == -1)
    perror("error on sigemptyset()");
  if (father)
    sigint.sa_handler = signal_handler;
  else
    sigint.sa_handler = signal_handler_child;
  sigint.sa_mask = smask;
  sigint.sa_flags = 0;

  if (sigaction(SIGINT, &sigint, NULL) == -1)
    perror("error on sigaction()");

  sigusr.sa_handler = unlock;
  sigusr.sa_mask = smask;
  sigusr.sa_flags = 0;

  if (sigaction(SIGINT, &sighup, NULL) == -1)
    perror("error on sigaction()");

  sigusr.sa_handler = sighup;
  sigusr.sa_mask = smask;
  sigusr.sa_flags = 0;

  if (sigaction(SIGUSR1, &sigusr, NULL) == -1)
    perror("error on sigaction()");

  nftot++; //Not sure where to put this

  //Parse the arguments lines
  for (int i = 1; i < argc; i++)
  {
    if (argv[i][0] == '-')
    {
      switch (argv[i][1])
      {
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
    }
    else
    {
      if (first_agr)
      {
        mode = strtol(argv[i], NULL, 8);
        first_agr = false;
      }
      else
      {
        if (argv[i][0] == '/')
          absolute_path = true;
        copy(argv[i], file_path);
        dir_i = i;
      }
    }
  }

  if (!absolute_path)
    concatenate(working_dir, file_path);
  else
    copy(file_path, working_dir);

  //TODO error handler for opendir
  if (recursive)
  {
    if ((dir = opendir(working_dir)) == NULL)
    {
      perror("Failed to open dir...");
      //error_handler();
    }
    else
    {
      while ((entry = readdir(dir)) != NULL)
      {
        //TODO should avoid calling the recursive for '.' and '..', but it should call for .smgh files
        if (entry->d_name[0] == '.')
          continue;
        char folder_r[256];
        copy(working_dir, folder_r);
        concatenate(folder_r, entry->d_name);
        father = false;
        if ((child = fork()) == -1)
          perror("Failed to fork()");
        if (child == 0)
        {
          argv[dir_i] = folder_r;
          if (verbose)
            printf("forking and calling main with argv = %s\n", argv[dir_i]);
          main(argc, argv);
          break;
        }
      }
      closedir(dir);
    }
  }
  global_file_path = working_dir;

  //sleep(5);

  if (verbose || verboseC)
    printf("changing file '%s' to %o\n", working_dir, mode);

  if (logs)
  {
    stat(working_dir, stat_buffer);
    mode_t oldPermission = stat_buffer->st_mode;

    char info[150];
    snprintf(info, sizeof(info), "%s : %o : %o", working_dir, oldPermission, mode);

    write_log("FILE_MODF", info);
  }
  return_code = chmod(working_dir, mode);
  nfmod++;
  if (return_code != 0)
    error_handler();

  //TODO error handler for child
  if (child != 0)
  {
    pid_t r = wait(NULL);
    if (verbose)
      printf("child with pid = %d has finished\n", r);
  }

  if (logs)
    write_log("PROC_EXIT", "0");

  return 0;
}

void signal_handler(int signo)
{
  if (log)
  {
    write_log("SIGNAL_RECV", "SIGINT");
  }
  printf("%d ; %s ; %d ; %d\n", getgid(), global_file_path, nftot, nfmod);

  if (!prompt())
  {
    if (log)
    {
      char info[25];
      snprintf(info, sizeof(info), "SIGUSR1 : %d", getpgrp());
      write_log("SIGNAL_SENT", info);
    }
    kill(-getpgrp(), SIGUSR1);
    return;
  }

  if (log)
  {
    char info[25];
    snprintf(info, sizeof(info), "SIGTERM : %d", getpgrp());
    write_log("SIGNAL_SENT", info);
  }
  kill(-getpgrp(), SIGHUP);

  if (logs)
    write_log("PROC_EXIT", "-2");
  exit(-2);
}

void signal_handler_child(int signo)
{
  if (log)
  {
    write_log("SIGNAL_RECV", "SIGINT");
  }

  printf("%d ; %s ; %d ; %d\n", getgid(), global_file_path, nftot, nfmod);
  pause();
  return;
}

void unlock()
{
  if (log)
  {
    write_log("SIGNAL_RECV", "SIGUSR1");
  }
  return;
}

void singnal_handler_hup()
{
  if (log)
  {
    write_log("SIGNAL_RECV", "SIGHUP");
  }

  if (logs)
    write_log("PROC_EXIT", "-3");
  exit(-3);
}

bool prompt()
{
  char response[10];
  while (1)
  {
    setbuf(stdout, NULL);
    printf("Are you sure you want to cancel? (y/n)\n");
    scanf("%s", response);
    if (response[0] == 'y' || response[0] == 'Y')
      return true;
    else if (response[0] == 'n' || response[0] == 'N')
      return false;
  }
}

void error_unknow_flag(char flag)
{
  printf("Error: Unknow flag \'%c\'\n", flag);

  if (logs)
    write_log("PROC_EXIT", "-4");
  exit(-4);
}

void error_handler()
{
  switch (errno)
  {
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

  if (logs)
    write_log("PROC_EXIT", "-5");
  exit(-5);
}

void write_log(char *event, char *info)
{
  //TO DO: Calculate the instant
  int instant = 0;
  FILE *log_file = fopen(log_dir, 'a');

  fprintf(log_file, "%d ; %d ; %s ; %s\n", instant, getpid(), event, info);
  fclose(log_dir);
}
