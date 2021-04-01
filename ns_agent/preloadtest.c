#define _GNU_SOURCE

#include <stdio.h>
#include <dlfcn.h>

typedef FILE *(*fopen_t)(const char *pathname, const char *mode);
fopen_t real_fopen;

FILE *fopen(const char *pathname, const char *mode) {
  fprintf(stderr, "called fopen(%s, %s)\n", pathname, mode);
  if (!real_fopen) {
    real_fopen = dlsym(RTLD_NEXT, "fopen");
  }

  return real_fopen(pathname, mode);
}

__attribute__((constructor)) static void setup(void) {
  fprintf(stderr, "called setup()\n");
}