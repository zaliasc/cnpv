#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "log.h"

#include "json.h"

extern char *program_invocation_short_name;

static void process_pair(json_value *pair) {
  if (pair->type != json_object) {
    log_err("parse pair failed");
    exit(-1); 
  }

  for (int i = 0; i < pair->u.array.length; i++) {
    if (!strcmp(pair->u.object.values[i].name, "PATH")) {
      log_info("1111");
    }
    else if (!strcmp(pair->u.object.values[i].name, "AUTHORITY")) {
      log_info("2222");
    }
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


char config_path[] = "/home/zhuzhicheng/project/cnpv/ns_agent/config.json";

void config_init()
{
  int fd;
  struct stat filestatus;
  int file_size;
  char *file_contents;
  json_char *json;
  json_value *value;

  if (stat(config_path, &filestatus) != 0)
  {
    log_err("File %s not found", config_path);
    exit(-1);
  }

  file_size = filestatus.st_size;
  file_contents = (char *)malloc(filestatus.st_size);
  if (file_contents == NULL)
  {
    log_err("Memory error: unable to allocate %d bytes", file_size);
    exit(-1);
  }

  fd = open(config_path, O_RDONLY);
  if (fd == 0)
  {
    log_err("Unable to open %s", config_path);
    free(file_contents);
    exit(-1);
  }
  if (read(fd, file_contents, file_size) == -1)
  {
    log_err("Unable t read content of %s", config_path);
    close(fd);
    free(file_contents);
    exit(-1);
  }
  close(fd);

  printf("%s\n", file_contents);

  json = (json_char *)file_contents;

  value = json_parse(json, file_size);

  if (value == NULL)
  {
    log_err("Unable to parse config_file!");
    free(file_contents);
    exit(1);
  }
  process_root(value);
  json_value_free(value);
  free(file_contents);
}

int main() {
  config_init();
  return 0;
}