#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define open1(path)                                                            \
  do {                                                                         \
    int fd = open(path, O_RDONLY | O_NONBLOCK, 0);                             \
    printf("%d  ", fd);                                                        \
    if (fd != -1)                                                              \
      close(fd);                                                               \
  } while (0)

int main() {
  open1("/home/zhuzhicheng/project/nginx_file/js/ecStat.min.js");
  open1("/home/zhuzhicheng/project/nginx_file/js/bmap.min.js");
  open1("/home/zhuzhicheng/project/nginx_file/js/dataTool.min.js");
  open1("/home/zhuzhicheng/project/nginx_file/js/echarts-gl.min.js");
  open1("/home/zhuzhicheng/project/nginx_file/js/echarts.min.js");
  open1("/home/zhuzhicheng/project/nginx_file/js/vue.min.js");
  open1("/home/zhuzhicheng/project/nginx_file/js/bootstrap.min.js");
  open1("/home/zhuzhicheng/project/nginx_file/js/popper.min.js");
  open1("/home/zhuzhicheng/project/nginx_file/js/jquery.min.js");
  open1("/home/zhuzhicheng/project/nginx_file/index.html");

  open1("/home/zhuzhicheng/project/nginx_file/js/ecStat.min.js");
  open1("/home/zhuzhicheng/project/nginx_file/js/bmap.min.js");
  open1("/home/zhuzhicheng/project/nginx_file/js/dataTool.min.js");
  open1("/home/zhuzhicheng/project/nginx_file/js/echarts-gl.min.js");
  open1("/home/zhuzhicheng/project/nginx_file/js/echarts.min.js");
  open1("/home/zhuzhicheng/project/nginx_file/js/vue.min.js");
  open1("/home/zhuzhicheng/project/nginx_file/js/bootstrap.min.js");
  open1("/home/zhuzhicheng/project/nginx_file/js/popper.min.js");
  open1("/home/zhuzhicheng/project/nginx_file/js/jquery.min.js");
  open1("/home/zhuzhicheng/project/nginx_file/index.html");

  return 0;
}