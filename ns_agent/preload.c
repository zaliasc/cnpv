#define _GNU_SOURCE

#include <stdio.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <stdarg.h>

typedef FILE *(*fopen_t)(const char *pathname, const char *mode);
typedef int (* open_t)(const char *pathname, int flags, ...);
typedef int (* open64_t)(const char *pathname, int flags, ...);
fopen_t real_fopen;
open_t real_open;
open64_t real_open64;

FILE *fopen(const char *pathname, const char *mode) {
  fprintf(stderr, "called fopen(%s, %s)\n", pathname, mode);
  if (!real_fopen) {
    real_fopen = dlsym(RTLD_NEXT, "fopen");
  }

  return real_fopen(pathname, mode);
}

int open64(const char *pathname, int flags, ...) {
  fprintf(stderr, "called open(%s)\n", pathname);
  if (!real_open) {
    real_open = dlsym(RTLD_NEXT, "open64");
  }
  if(flags & O_CREAT) {
    va_list v;
    va_start(v, flags);
    mode_t mode;
    mode = va_arg(v, mode_t);
    return real_open64(pathname, flags, mode); 
  }
  else return real_open64(pathname, flags);
}

int open(const char *pathname, int flags, ...) {
  fprintf(stderr, "called open(%s)\n", pathname);
  if (!real_open) {
    real_open = dlsym(RTLD_NEXT, "open");
  }
  if(flags & O_CREAT) {
    va_list v;
    va_start(v, flags);
    mode_t mode;
    mode = va_arg(v, mode_t);
    return real_open(pathname, flags, mode); 
  }
  else return real_open(pathname, flags);
}

__attribute__((constructor)) static void setup(void) {
  fprintf(stderr, "called setup()\n");
}