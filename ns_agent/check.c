#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#include "log.h"
#include "types.h"
#include "hashmap.h"

extern struct hashmap *map;

bool check_permission(const char * pathname, int oflag) {

  char buf[MAX_PATH];

  if (!strstr(pathname, "/")) {
    char * b = getcwd(NULL, 0);
    sprintf(buf, "%s/%s", b, pathname);
    log_info("pathname: %s", buf);
    free(b);
  }

  struct user *user;
  struct user tmp = {.permission = 0};
  strncpy(tmp.pathname, buf, MAX_PATH-1);
  // strncpy(tmp.pathname, pathname, MAX_PATH-1);
  user = hashmap_get(map, &tmp);
  if (!user) return true;
  
  int permission = user->permission;
  
  if (permission & O_FORBIDDEN) return false;
  if (oflag & O_RDONLY) return !!(permission & O_READ);
  if (oflag & O_WRONLY) return !!(permission & O_WRITE);
  if (oflag & O_RDWR) return (!!(permission & O_READ)) && (!!(permission & O_WRITE));

  return true;
}