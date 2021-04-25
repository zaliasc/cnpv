#ifndef CACHE_H
#define CACHE_H

#include "log.h"
#include "lru.h"

// cache count = 64
#define CACHE_SIZE (64 * 1024)  // 64k
#define AVG_SIZE (1024)         // 1k

void cache_init();

void cache_insert(const char* key, const char* value);

void cache_get(char* key, char** ret);

#endif  // CACHE_H