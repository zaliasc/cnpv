#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/printk.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/stat.h>
#include <linux/uaccess.h>
#include <linux/unistd.h>

#include "json.h"
#include "log.h"
#include "utils.h"

static void process_pair(json_value *pair) {
  if (pair->type != json_object) {
    log_err("parse pair failed");
  }

  for (int i = 0; i < pair->u.array.length; i++) {
    if (!strcmp(pair->u.object.values[i].name, "PATH")) {
      log_info("1111");
    } else if (!strcmp(pair->u.object.values[i].name, "AUTHORITY")) {
      log_info("2222");
    }
  }
}

static void process_array(json_value *array) {
  if (array->type != json_array) {
    log_err("parse array failed");
  }

  for (int i = 0; i < array->u.array.length; i++) {
    process_pair(array->u.array.values[i]);
  }
}

static void process_root(json_value *root) {
  if (root->type != json_object) {
    log_err("parse root failed");
  }

  for (int i = 0; i < root->u.object.length; i++) {
    // if (strstr(root->u.object.values[i].name, program_invocation_short_name))
    // {
    process_array(root->u.object.values[i].value);
    // }
  }

  return;
}

char config_path[] = "/home/zhuzhicheng/project/cnpv/ns_agent/config.json";

void config_init(void) {

  int file_size;
  char *file_contents;

  file_contents = load_file(config_path, file_size);

  printk("file: %s/n  file_size: %d/n file contents: %s", config_path,
         file_size, file_contents);

  // json_char *json;

  // json_value *value;

  // json = (json_char *)file_contents;

  // value = json_parse(json, file_size);

  // if (value == NULL) {
  //   log_err("Unable to parse config_file!");
  //   kfree(file_contents);
  // }
  // process_root(value);
  // json_value_free(value);
  kfree(file_contents);
}