#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

int open(const char *path, int oflag, ...) {
  printf("call myopen here! %s\n", path);
  return 0;
}