#define _GNU_SOURCE

#include <dlfcn.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>

typedef FILE *(*fopen_t)(const char *pathname, const char *mode);
typedef int (*open_t)(const char *pathname, int flags, ...);
typedef int (*open64_t)(const char *pathname, int flags, ...);
typedef int (*openat_t)(int dirfd, const char *pathname, int flags, ...);
typedef int (*openat64_t)(int dirfd, const char *pathname, int flags, ...);

fopen_t real_fopen;
open_t real_open;
open64_t real_open64;
openat_t real_openat;
openat64_t real_openat64;

FILE *fopen(const char *pathname, const char *mode) {
  fprintf(stderr, "called fopen(%s, %s)\n", pathname, mode);
  if (!real_fopen) {
    real_fopen = dlsym(RTLD_NEXT, "fopen");
  }

  return real_fopen(pathname, mode);
}

int open64(const char *pathname, int flags, ...) {
  fprintf(stderr, "called open64(%s)\n", pathname);
  if (!real_open) {
    real_open64 = dlsym(RTLD_NEXT, "open64");
  }
  if (__OPEN_NEEDS_MODE(flags)) {
    va_list v;
    va_start(v, flags);
    mode_t mode;
    mode = va_arg(v, mode_t);
    return real_open64(pathname, flags, mode);
  } else
    return real_open64(pathname, flags);
}

int open(const char *pathname, int flags, ...) {
  fprintf(stderr, "called open(%s)\n", pathname);
  if (!real_open) {
    real_open = dlsym(RTLD_NEXT, "open");
  }
  if (__OPEN_NEEDS_MODE(flags)) {
    va_list v;
    va_start(v, flags);
    mode_t mode;
    mode = va_arg(v, mode_t);
    return real_open(pathname, flags, mode);
  } else
    return real_open(pathname, flags);
}

int openat(int dirfd, const char *pathname, int flags, ...) {
  fprintf(stderr, "called openat(%s)\n", pathname);
  if (!real_openat) {
    real_openat = dlsym(RTLD_NEXT, "openat");
  }
  if (__OPEN_NEEDS_MODE(flags)) {
    va_list v;
    va_start(v, flags);
    mode_t mode;
    mode = va_arg(v, mode_t);
    return real_openat(dirfd, pathname, flags, mode);
  } else
    return real_openat(dirfd, pathname, flags);
}

int openat64(int dirfd, const char *pathname, int flags, ...) {
  fprintf(stderr, "called openat64(%s)\n", pathname);
  if (!real_openat64) {
    real_openat64 = dlsym(RTLD_NEXT, "openat64");
  }
  if (__OPEN_NEEDS_MODE(flags)) {
    va_list v;
    va_start(v, flags);
    mode_t mode;
    mode = va_arg(v, mode_t);
    return real_openat64(dirfd, pathname, flags, mode);
  } else
    return real_openat64(dirfd, pathname, flags);
}

__attribute__((constructor)) static void setup(void) {
  fprintf(stderr, "called setup()\n");
}