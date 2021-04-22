#include <dlfcn.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "hashmap.h"
#include "log.h"
#include "types.h"

extern struct hashmap *map;

extern char *cwd;

bool check_permission(const char *pathname, int oflag) {

  char buf[MAX_PATH];

  if (!strstr(pathname, "/")) {
    sprintf(buf, "%s/%s", cwd, pathname);
    log_info("pathname: %s", buf);
  }

  struct user *user;
  struct user tmp = {.permission = 0};
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-truncation"
  strncpy(tmp.pathname, buf, MAX_PATH - 1);
#pragma GCC diagnostic pop
  // strncpy(tmp.pathname, pathname, MAX_PATH-1);
  user = hashmap_get(map, &tmp);
  if (!user)
    return true;

  int permission = user->permission;

  if (oflag == FOPEN_MODE_FLAG)
    return true;
  if (permission & O_FORBIDDEN)
    return false;
  if (oflag & O_RDONLY)
    return !!(permission & O_READ);
  if (oflag & O_WRONLY)
    return !!(permission & O_WRITE);
  if (oflag & O_RDWR)
    return (!!(permission & O_READ)) && (!!(permission & O_WRITE));

  return true;
}