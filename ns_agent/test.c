#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

int main() {
  open("/home/zhuzhicheng/project/nodejs/node-v15.0.0-linux-x64.tar.gz",O_RDWR);
  return 0;
}