#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

int main() {
  int fd = open("agent.c", 00|0 ,0);
  printf("%d\n",fd);
  char buf[100] = {0};
  ssize_t t = read(fd, buf, 10);
  printf("buf:%s\n",buf);
  return 0;
}