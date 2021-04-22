#ifndef CACHE_H
#define CACHE_H

#include "log.h"
#include "lru.h"

// #define USE_CACHE

#define CACHE_SIZE (8 * 1024)  // 8k
#define AVG_SIZE (2 * 1024)    // 2k

void cache_init();

void cache_insert(const char* key, const char* value);

void cache_get(char* key, char** ret);

#endif