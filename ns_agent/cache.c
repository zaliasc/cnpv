#include "cache.h"
#include <stdlib.h>
#include <string.h>

lruc *cache;

void copy_key_val(lruc *cache, const char *key, const char *val) {
  int error = 0;
  char *new_key = (char *)malloc(strlen(key) + 1);
  strcpy(new_key, key);
  char *new_val = (char *)malloc(strlen(val) + 1);
  strcpy(new_val, val);
  if ((error = lruc_set(cache, new_key, strlen(key) + 1, new_val, AVG_SIZE)))
    log_err("Error in set: %i\n", error);
}

void cache_init() {
  cache = lruc_new(CACHE_SIZE, AVG_SIZE);
  if (cache == NULL) {
    log_err("init cache failed!");
    exit(-1);
  }
}

void cache_insert(const char *key, const char *value) {
  log_info("cache insert key: %s , cache value: %s", key, value);
  copy_key_val(cache, key, value);
}

void cache_get(char *key, char **ret) {
  int error = 0;
  if ((error = lruc_get(cache, key, strlen(key) + 1, (void **)(ret)))) {
    log_err("Error in get: %i\n", error);
    exit(-1);
  }
  log_info("cache get key: %s , cache value: %s", key, (char *)(*ret));
}