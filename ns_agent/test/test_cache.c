#include "cache.h"
#include <stdio.h>

#define test(key, value)                                                       \
  do {                                                                         \
    cache_get(key, &value);                                                    \
    printf("%s:%s\n", key, value);                                             \
  } while (0)

int main() {
  cache_init();
  cache_insert("key1", "value1");
  cache_insert("key2", "value2");
  cache_insert("key3", "value3");
  cache_insert("key4", "value4");
  cache_insert("key5", "value5");
  cache_insert("key6", "value6");
  cache_insert("key7", "value7");
  cache_insert("key8", "value7");
  cache_insert("key9", "value7");
  cache_insert("key10", "value7");
  cache_insert("key11", "value7");
  cache_insert("key12", "value7");
  cache_insert("key13", "value7");
  cache_insert("key14", "value7");
  char *value;
  test("key1", value);
  test("key2", value);
  test("key3", value);
  test("key4", value);
  test("key5", value);
  test("key6", value);
  test("key7", value);
  test("key8", value);
  test("key9", value);
  test("key10", value);
  test("key11", value);
  test("key12", value);
  test("key13", value);
  test("key14", value);
  return 0;
}