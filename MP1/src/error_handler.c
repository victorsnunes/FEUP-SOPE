#include "error_handler.h"

void error_handler() {
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

  if (logs)
    write_log("PROC_EXIT", "-5");

  exit(-5);
}