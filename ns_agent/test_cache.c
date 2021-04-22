#include "cache.h"
#include <stdio.h>

int main() {
  cache_init();
  cache_insert("key", "value");
  char *value;
  cache_get("key", &value);
  printf("key : key, value : %s", value);
  return 0;
}