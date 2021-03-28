#include <stdbool.h>
#include <string.h>

#include "log.h"
#include "types.h"
#include "hashmap.h"

extern struct hashmap *map;

bool check_permission(const char * pathname) {
  struct user *user;
  struct user tmp = {.permission = 0};
  strncpy(tmp.pathname, pathname, MAX_PATH-1);
  user = hashmap_get(map, &tmp);
  if (!user) return false;
  return true;
}