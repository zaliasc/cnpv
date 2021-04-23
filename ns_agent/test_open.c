#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
  int fd = open("agent.c", O_RDONLY, 0);
  printf("%d\n", fd);
  char buf[100] = {0};
  if (fd != -1) {

    ssize_t t = read(fd, buf, 10);
    printf("buf:%s\n", buf);
  }
  return 0;
}