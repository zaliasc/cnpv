#include <dlfcn.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "cache.h"
#include "hashmap.h"
#include "log.h"
#include "types.h"

extern struct hashmap *map;

extern char *cwd;

bool check_permission(const char *pathname, int oflag) {
  bool ret = false;
  char buf[MAX_PATH];
  int permission = -1;
  char *cache_value = NULL;

  if (!strstr(pathname, "/")) {
    sprintf(buf, "%s/%s", cwd, pathname);
    log_debug("pathname: %s", buf);
  } else {
    strcpy(buf, pathname);
  }

#ifdef USE_CACHE
  cache_get(buf, &cache_value); // A or F
  if (cache_value) {
    switch (*cache_value) {
    case 'A':
      return true;
    case 'F':
      return false;
    default:
      break;
    }
  }
#endif

  struct user *user;
  struct user tmp = {.permission = 0};
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-truncation"
  strncpy(tmp.pathname, buf, MAX_PATH - 1);
#pragma GCC diagnostic pop

  user = hashmap_get(map, &tmp);

  if (!user) {
#ifdef USE_CACHE
    cache_insert(buf, "A");
#endif
    return true;
  }

  permission = user->permission;

  log_debug("permission: %d  oflag: %d", permission, oflag);

  if (oflag == FOPEN_MODE_FLAG)
    ret = true;
  else if (permission & O_FORBIDDEN)
    ret = false;
  else if (oflag & 0x03 == O_RDONLY)
    ret = !!(permission & O_READ);
  else if (oflag & 0x03 == O_WRONLY)
    ret = !!(permission & O_WRITE);
  else if (oflag & 0x03 == O_RDWR)
    ret = (!!(permission & O_READ)) && (!!(permission & O_WRITE));

#ifdef USE_CACHE
  if (ret == true)
    cache_insert(buf, "A");
  else
    cache_insert(buf, "F");
#endif

  return ret;
}