#include "xmod.h"

int main(int argc, char *argv[]) {
  start_t = clock();
  bool first_agr = true;

  // Flags
  bool verbose = false;
  bool verboseC = false;
  bool recursive = false;
  bool absolute_path = false;

  //Aux vars
  int setMode = 0;
  extern int oldperms;

  __mode_t mode = 0;
  char file_path[256];
  char working_dir[256];
  snprintf(working_dir, sizeof(working_dir), "%s", getenv("PWD"));
  int return_code = 0;
  int dir_i;

  struct dirent *entry;
  DIR *dir;

  struct sigaction sigint, sigusr, sighup;
  sigset_t smask;

  log_dir = getenv("LOG_FILENAME");

  if (log_dir == NULL)
    logs = false;

  if (verbose && father) printf("log path: %s\n", log_dir);

  if (father && logs) fclose(fopen(log_dir, "w"));

  // Writes the log for process created
  if (logs) {
    char commandline_args[512];
    snprintf(commandline_args, sizeof(commandline_args), "%s", argv[0]);

    for (int i = 1; i < argc; i++) {
      strcat(commandline_args, " : ");
      strcat(commandline_args, argv[i]);
    }

    write_log("PROC_CREAT", commandline_args);
  }

  // FIXME check if getenv is working
  if (working_dir == NULL) {
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


  sighup.sa_handler = signal_handler_hup;
  sighup.sa_mask = smask;
  sighup.sa_flags = 0;
  if (sigaction(SIGTERM, &sighup, NULL) == -1)
    perror("error on sigaction()");

  if (sigaction(SIGUSR1, &sigusr, NULL) == -1)
    perror("error on sigaction()");

  nftot++;

  // Parse the arguments lines
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-' && argv[i][1] != 'r' && argv[i][1] != 'x' && argv[i][1] != 'w') {
      switch (argv[i][1]) {
      case 'v':
        verbose = true;
        break;
      case 'c':
        verboseC = true;
        break;
      case 'R':
        recursive = true;
        break;
        default:
          break;
      }
    } else {
      if (first_agr) {
         if(isdigit(argv[i][0])) {
          //printf("Octal Mode \n");
          mode = strtol(argv[i], NULL, 8);
        } else {
          //printf("Normal Mode \n");
          mode_state_machine(argv[i]);
          setMode = 1;
        }
        first_agr = false;
      } else {
        if (argv[i][0] == '/')
          absolute_path = true;
        snprintf(file_path, sizeof(file_path), "%s", argv[i]);
        dir_i = i;
      }
    }
  }

  if (!absolute_path)
    concatenate(working_dir, file_path);
  else
    snprintf(working_dir, sizeof(working_dir), "%s", file_path);

  if(setMode == 1){
    oldperms = getPermissions(working_dir);
    mode = changePermissions(oldperms);
  }

  if (recursive) {
    if ((dir = opendir(working_dir)) == NULL) {
      if (verbose) printf("failed to open file '%s'\n", working_dir);
    } else {
      while ((entry = readdir(dir)) != NULL) {
        if ((strcmp(entry->d_name, ".") == 0) ||
            (strcmp(entry->d_name, "..") == 0))
          continue;
        char folder_r[256];
        snprintf(folder_r, sizeof(folder_r), "%s", working_dir);
        concatenate(folder_r, entry->d_name);

        if ((child = fork()) == -1)
          perror("Failed to fork()");

        if (child == 0) {
          father = false;
          argv[dir_i] = folder_r;
          if (verbose)
            printf("forking and calling main with argv = %s\n", argv[dir_i]);
          main(argc, argv);
          exit(0);
        }
      }
      closedir(dir);
    }
  }

  global_file_path = working_dir;

  struct stat *stat_buffer = (struct stat *) malloc(sizeof(struct stat));

  if ((return_code = stat(working_dir, stat_buffer)) != 0) {
    perror("error stat()");
    error_handler();
  }
  mode_t old_permission = stat_buffer->st_mode;

  old_permission %= 01000;

  if (verbose || verboseC)
    printf("mode of '%s' change from '%o' to '%o'\n",
            working_dir, old_permission, mode);

  if (logs) {
    char info[286];
    snprintf(info, sizeof(info), "%s : 0%o : 0%o",
             working_dir, old_permission, mode);

    write_log("FILE_MODF", info);
  }
  return_code = chmod(working_dir, mode);
  nfmod++;
  if (return_code != 0)
    error_handler();

  free(stat_buffer);

  if (child != 0) {
    pid_t r = wait(NULL);
    if (verbose)
      printf("child with pid = %d has finished\n", r);
  }

  if (logs)
    write_log("PROC_EXIT", "0");

  return 0;
}

void signal_handler(int signo) {
  if (logs) {
    write_log("SIGNAL_RECV", "SIGINT");
  }
  printf("%d ; %s ; %d ; %d\n", getpid(), global_file_path, nftot, nfmod);

  if (!prompt()) {
    if (logs) {
      char info[25];
      snprintf(info, sizeof(info), "SIGUSR1 : %d", getpgrp());
      write_log("SIGNAL_SENT", info);
    }
    kill(-getpgrp(), SIGUSR1);
    return;
  }

  if (logs) {
    char info[25];
    snprintf(info, sizeof(info), "SIGTERM : %d", getpgrp());
    write_log("SIGNAL_SENT", info);
  }

  pid_t group = getpgrp();
  if (group == -1 ) perror("erro getpgrp()");

  if (kill(-group, SIGTERM) != 0) {
    perror("erro kill()");
  }
  printf("working...\n");

  if (logs)
    write_log("PROC_EXIT", "-3");
  exit(-3);
}

void signal_handler_child() {
  if (logs) {
    write_log("SIGNAL_RECV", "SIGINT");
  }

  printf("%d ; %s ; %d ; %d\n", getpid(), global_file_path, nftot, nfmod);
  pause();
  return;
}

void unlock() {
  if (logs)
    write_log("SIGNAL_RECV", "SIGUSR1");

  return;
}

void signal_handler_hup() {
  if (logs)
    write_log("SIGNAL_RECV", "SIGTERM");

  if (logs)
    write_log("PROC_EXIT", "-3");

  exit(-3);
}

bool prompt() {
  char response[10];
  while (1) {
    setbuf(stdout, NULL);
    printf("Are you sure you want to cancel? (y/n)\n");
    scanf("%s", response);
    if (response[0] == 'y' || response[0] == 'Y')
      return true;
    else if (response[0] == 'n' || response[0] == 'N')
      return false;
  }
}
