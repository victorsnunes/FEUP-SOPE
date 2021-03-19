#include "util.h"

//Concatenate in the format home/file and return it through file
void concatenate (char home[], char file[]) {
  int size_a = 0;
  int size_b = 0;

  while (home[size_a] != '\0') {
    size_a++;
  }

  if (home[size_a - 1] != '/') {
    home[size_a] = '/';
    size_a++;
  }

  while (file[size_b] != '\0') {
    home[size_a] = file[size_b];
    size_a++;
    size_b++;
  }
  home[size_a] = '\0';
}

//Copy what is inside a to b
void copy (char a[], char b[]) {
  int i;
  for (i = 0; a[i] != '\0'; i++){
    b[i] = a[i];
  }
  b[i] = '\0';
}
