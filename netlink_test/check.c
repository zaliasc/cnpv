#include <linux/string.h>

#include "hashmap.h"
#include "log.h"
#include "types.h"

extern struct hashmap *map;

bool check_permission(const char *pathname, int oflag) {
  bool ret = false;
  char buf[MAX_PATH];
  int permission = -1;

  strcpy(buf, pathname);

  struct mapuser *user;
  struct mapuser tmp = {.permission = 0};
  strncpy(tmp.pathname, buf, MAX_PATH - 1);

  user = hashmap_get(map, &tmp);

  if (!user) {
    return true;
  }

  permission = user->permission;

  log_debug("pathname : %s , permission: %d  oflag: %d", permission, oflag);

  return true;

  // if (permission & O_FORBIDDEN)
  //   ret = false;
  // else if ((oflag & 0x03) == O_RDONLY)
  //   ret = !!(permission & O_READ);
  // else if ((oflag & 0x03) == O_WRONLY)
  //   ret = !!(permission & O_WRITE);
  // else if ((oflag & 0x03) == O_RDWR)
  //   ret = (!!(permission & O_READ)) && (!!(permission & O_WRITE));
  // return ret;
}