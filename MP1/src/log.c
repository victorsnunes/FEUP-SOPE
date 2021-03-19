#include "log.h"

void write_log(char *event, char *info) {
  clock_t end_t = clock();
  float instant = (double) (end_t - start_t)/(CLOCKS_PER_SEC*100);
  FILE *log_file = fopen(log_dir, "a");
  if(log_file == NULL) {
    perror("erro fopen()");
    error_handler();
    }
  fprintf(log_file, "%f ; %d ; %s ; %s\n", instant, getpid(), event, info);
  fclose(log_file);
}
