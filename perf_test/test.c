#include <dlfcn.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv) {
  int count = atoi(argv[1]);
  for (int i = 0; i < count; i++) {
    int fd = open("/home/zhuzhicheng/project/cnpv/perf_test/test.c", O_RDONLY, 0);
    close(fd);
  }
  return 0;
}