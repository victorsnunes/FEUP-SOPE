#include "log.h"

void write_log(char *event, char *info) {
  // TODO: Calculate the instant
  int instant = 0;
  FILE *log_file = fopen(log_dir, "a");
  if(log_file == NULL) {
    perror("erro fopen()");
    error_handler();
    }
  fprintf(log_file, "%d ; %d ; %s ; %s\n", instant, getpid(), event, info);
  fclose(log_file);
}