#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
  int fd = open("agent.c", 00 | 0, 0);
  printf("%d\n", fd);
  char buf[100] = {0};
  ssize_t t = read(fd, buf, 10);
  printf("buf:%s\n", buf);
  return 0;
}