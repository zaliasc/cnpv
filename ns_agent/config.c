#include <unistd.h>
#include <stdio.h>
#include <jansson.h>
#include <errno.h>
#include <string.h>
// #include <fcntl.h>

#include "log.h"
#include "types.h"
#include "hashmap.h"

struct hashmap *map;

extern char *program_invocation_short_name;

extern struct open_calls real_open;

static int get_mode_value(const char * s) {
  int ret = 0;
  if (strstr(s,"R") || strstr(s,"r")) ret |= O_READ;
  if (strstr(s,"W") || strstr(s,"w")) ret |= O_WRITE;
  if (strstr(s,"X") || strstr(s,"x")) ret |= O_EXEC;
  return ret;
}

// char config_path[] = "/home/zhuzhicheng/project/cnpv/ns_agent/config.json";
extern char * config_path;

int user_compare(const void *a, const void *b, void *udata) {
    const struct user *ua = a;
    const struct user *ub = b;
    return strcmp(ua->pathname, ub->pathname);
}

bool user_iter(const void *item, void *udata) {
    const struct user *user = item;
    printf("%s (age=%d)\n", user->pathname, user->permission);
    return true;
}

uint64_t user_hash(const void *item, uint64_t seed0, uint64_t seed1) {
    const struct user *user = item;
    return hashmap_sip(user->pathname, strlen(user->pathname), seed0, seed1);
}

void config_init() {
  log_info("config_init()");

  map = hashmap_new(sizeof(struct user), 0, 0, 0, 
                    user_hash, user_compare, NULL);

  int fd = real_open.open_call(config_path, O_RDONLY);

  log_info("json_loadfd()");

  json_t * root = json_loadfd(fd, 0, NULL);

  if (!root) {
    log_fatal("load config file failed!\n");
    exit(-1);
  }

  void * iter = json_object_iter(root);

  if (!iter) {
    log_fatal("load config file failed!\n");
    exit(-1);
  }

  while (iter != NULL) {
    const char * program_name = json_object_iter_key(iter);
    if (strstr(program_name, program_invocation_short_name)) {
      log_info("%s : %s\n", program_name, program_invocation_short_name);
      json_t * array = json_object_iter_value(iter);

      int index;
      json_t * object;
      json_array_foreach(array, index, object) {

        char * key;
        json_t * value;
        struct user tmp = {.pathname = {0}};
        json_object_foreach(object, key, value){
          if (!strcmp(key, "PATH")) {
            strncpy(tmp.pathname, json_string_value(value), MAX_PATH - 1);
          }
          else if (!strcmp(key, "AUTHORITY")) {
            tmp.permission = get_mode_value(json_string_value(value));
          }
        }
        hashmap_set(map, &tmp);
        // hashmap_set(map, &(struct user){ .name="Dale", .age=44 });
      }

    }
    iter = json_object_iter_next(root, iter);
  }

  log_info("hash map count : %d", (int)hashmap_count(map));

  return ;
}