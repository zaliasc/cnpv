#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include "log.h"
#include <unistd.h>
#include <errno.h>
#include "agent.h"

extern char *program_invocation_name;
extern char *program_invocation_short_name;

int main() {
  log_info("start!\n");
  int fd = open("/home/zhuzhicheng/project/cnpv/ns_agent/config.c",O_RDWR);
  printf("%d\n",fd);
  char buf[100] = {0};
  ssize_t t = read(fd, buf, 10);
  printf("buf:%s\n",buf);
  printf("program: %s\n",program_invocation_short_name);
  return 0;
}

fopen()