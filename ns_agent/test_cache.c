#include "cache.h"
#include <stdio.h>

int main() {
  cache_init();
  cache_insert("key1", "value1");
  cache_insert("key2", "value2");
  cache_insert("key3", "value3");
  char *value;
  cache_get("key1", &value);
  printf("key1 : key1, value1 : %s", value);
  cache_get("key2", &value);
  printf("key2 : key2, value2 : %s", value);
  cache_get("key3", &value);
  printf("key3 : key3, value3 : %s", value);
  return 0;
}