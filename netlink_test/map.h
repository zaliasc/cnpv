#ifndef MAP_H
#define MAP_H

#include <linux/printk.h>
#include <linux/stddef.h>
#include <linux/types.h>

typedef unsigned long size_t;
// typedef unsigned long uint64_t;

struct hashmap;

struct hashmap *
hashmap_new(size_t elsize, size_t cap, uint64_t seed0, uint64_t seed1,
            uint64_t (*hash)(const void *item, uint64_t seed0, uint64_t seed1),
            int (*compare)(const void *a, const void *b, void *udata),
            void *udata);
void hashmap_free(struct hashmap *map);
void hashmap_clear(struct hashmap *map, bool update_cap);
size_t hashmap_count(struct hashmap *map);
bool hashmap_oom(struct hashmap *map);
void *hashmap_get(struct hashmap *map, void *item);
void *hashmap_set(struct hashmap *map, void *item);
void *hashmap_delete(struct hashmap *map, void *item);
void *hashmap_probe(struct hashmap *map, uint64_t position);
bool hashmap_scan(struct hashmap *map,
                  bool (*iter)(const void *item, void *udata), void *udata);
void hashmap_set_allocator(void *(*malloc)(size_t), void (*free)(void *));
uint64_t hashmap_sip(const void *data, size_t len, uint64_t seed0,
                     uint64_t seed1);
uint64_t hashmap_murmur(const void *data, size_t len, uint64_t seed0,
                        uint64_t seed1);

#endif