#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <dlfcn.h>
#include <fcntl.h>

#include "log.h"
#include "types.h"
#include "hashmap.h"

extern struct hashmap *map;

bool check_permission(const char * pathname, int oflag) {
  struct user *user;
  struct user tmp = {.permission = 0};
  strncpy(tmp.pathname, pathname, MAX_PATH-1);
  user = hashmap_get(map, &tmp);
  if (!user) return false;
  
  int permission = user->permission;
  if (oflag & O_RDONLY) return !!(permission & O_READ);
  if (oflag & O_WRONLY) return !!(permission & O_WRITE);
  if (oflag & O_RDWR) return (!!(permission & O_READ)) && (!!(permission & O_WRITE));

  return true;
}