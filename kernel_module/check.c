#include "map.h"
#include "types.h"
#include <linux/printk.h>
#include <linux/string.h>

extern struct hashmap *map;

int check_permission(const char *pathname, int oflag) {
  char buf[MAX_PATH];
  int permission = -1;

  strcpy(buf, pathname);

  struct mapuser *user;
  struct mapuser tmp = {.permission = 0};
  strncpy(tmp.pathname, buf, MAX_PATH - 1);

  user = hashmap_get(map, &tmp);

  if (!user) {
    return 1;
  }

  permission = user->permission;

  printk("pathname : %s , permission: %d  oflag: %d", pathname, permission,
         oflag);

  int ret = 1;

  if (permission & O_FORBIDDEN)
    ret = false;
  else if ((oflag & 0x03) == O_RDONLY)
    ret = !!(permission & O_READ);
  else if ((oflag & 0x03) == O_WRONLY)
    ret = !!(permission & O_WRITE);
  else if ((oflag & 0x03) == O_RDWR)
    ret = (!!(permission & O_READ)) && (!!(permission & O_WRITE));
  return ret;
}