#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "hashmap.h"
#include "json.h"
#include "log.h"
#include "types.h"

extern struct real_calls real_call;
extern char *program_invocation_short_name;
extern char *config_path;

// hashmap
struct hashmap *map;

struct user user_t = {.pathname = {0}};

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

// config
int get_mode_value(const char *s) {
  int ret = 0;
  if (strstr(s, "F") || strstr(s, "f"))
    ret = O_FORBIDDEN;
  if (strstr(s, "R") || strstr(s, "r"))
    ret |= O_READ;
  if (strstr(s, "W") || strstr(s, "w"))
    ret |= O_WRITE;
  return ret;
}

void get_dir_content(char *path, int permission) {
  log_debug("process dir path");
  DIR *d = opendir(path);
  if (d == NULL)
    return;
  struct dirent *dir;
  while ((dir = readdir(d)) != NULL) {
    // if the type is not directory just print
    if (dir->d_type != DT_DIR) {
      sprintf(user_t.pathname, "%s%s", path, dir->d_name);
      user_t.permission = permission;
      hashmap_set(map, &user_t);
      log_debug("%s%s\n", path, dir->d_name);
    } else if (dir->d_type == DT_DIR && strcmp(dir->d_name, ".") != 0 &&
               strcmp(dir->d_name, "..") != 0) {
      // if it is a directory
      char d_path[512];
      sprintf(d_path, "%s%s", path, dir->d_name);
      get_dir_content(d_path, permission);
    }
  }
  closedir(d);
}

static void process_pair(json_value *pair) {
  if (pair->type != json_object) {
    log_err("parse pair failed");
    exit(-1);
  }

  struct user tmp = {.pathname = {0}};

  for (int i = 0; i < pair->u.array.length; i++) {
    if (!strcmp(pair->u.object.values[i].name, "PATH")) {
      strncpy(tmp.pathname, pair->u.object.values[i].value->u.string.ptr,
              MAX_PATH - 1);
    } else if (!strcmp(pair->u.object.values[i].name, "AUTHORITY")) {
      tmp.permission =
          get_mode_value(pair->u.object.values[i].value->u.string.ptr);
    }
  }

  if (tmp.pathname[strlen(tmp.pathname) - 1] == '/') {
    // dir
    get_dir_content(tmp.pathname, tmp.permission);
  } else {
    hashmap_set(map, &tmp);
  }
}

static void process_array(json_value *array) {
  if (array->type != json_array) {
    log_err("parse array failed");
    exit(-1);
  }

  for (int i = 0; i < array->u.array.length; i++) {
    process_pair(array->u.array.values[i]);
  }
}

static void process_root(json_value *root) {
  if (root->type != json_object) {
    log_err("parse root failed");
    exit(-1);
  }

  for (int i = 0; i < root->u.object.length; i++) {
    if (strstr(root->u.object.values[i].name, program_invocation_short_name)) {
      process_array(root->u.object.values[i].value);
    }
  }

  return;
}

void config_init() {
  log_info("config_init()");

  int fd;
  struct stat filestatus;
  int file_size;
  char *file_contents;
  json_char *json;
  json_value *value;

  if (stat(config_path, &filestatus) != 0) {
    log_err("File %s not found", config_path);
    exit(-1);
  }

  file_size = filestatus.st_size;
  file_contents = (char *)malloc(filestatus.st_size);
  if (file_contents == NULL) {
    log_err("Memory error: unable to allocate %d bytes", file_size);
    exit(-1);
  }

  fd = real_call.real_open(config_path, O_RDONLY);
  if (fd == 0) {
    log_err("Unable to open %s", config_path);
    free(file_contents);
    exit(-1);
  }
  if (read(fd, file_contents, file_size) == -1) {
    log_err("Unable t read content of %s", config_path);
    close(fd);
    free(file_contents);
    exit(-1);
  }
  close(fd);

  json = (json_char *)file_contents;

  value = json_parse(json, file_size);

  if (value == NULL) {
    log_err("Unable to parse config_file!");
    free(file_contents);
    exit(1);
  }

  map =
      hashmap_new(sizeof(struct user), 0, 0, 0, user_hash, user_compare, NULL);

  process_root(value);

  log_info("hash map count : %d", (int)hashmap_count(map));

  json_value_free(value);
  free(file_contents);

  log_info("config_init() finish");

  return;
}
