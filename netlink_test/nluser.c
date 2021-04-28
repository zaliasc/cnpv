#include "json.h"
#include "log.h"
#include "types.h"
#include <dirent.h>
#include <fcntl.h>
#include <linux/netlink.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

#define NETLINK_USER 31

#define MAX_PAYLOAD 1024 /* maximum payload size*/
struct sockaddr_nl src_addr, dest_addr;
struct nlmsghdr *nlh = NULL;
struct iovec iov;
int sock_fd;
struct msghdr msg;
struct myuser user_t;

char config_path[255] = "config.json";

char target[255] = "a.out";

char *getfile_content(int *file_size) {
  int fd;
  struct stat filestatus;
  char *file_contents;

  if (stat(config_path, &filestatus) != 0) {
    printf("File %s not found", config_path);
    exit(-1);
  }

  *file_size = filestatus.st_size;
  file_contents = (char *)malloc(filestatus.st_size);
  if (file_contents == NULL) {
    printf("Memory error: unable to allocate %d bytes", *file_size);
    exit(-1);
  }

  fd = open(config_path, O_RDONLY);
  if (fd == 0) {
    free(file_contents);
    printf("file %s open failed", config_path);
    exit(-1);
  }

  if (read(fd, file_contents, *file_size) == -1) {
    printf("read failed");
    free(file_contents);
    exit(-1);
  }
  close(fd);

  return file_contents;
}

void sendstruct(struct myuser *u);

void sendcmd(struct myuser *u);

void get_dir_content(char *path, int permission) {
  log_debug("process dir path");
  DIR *d = opendir(path);
  if (d == NULL)
    return;
  struct dirent *dir;
  while ((dir = readdir(d)) != NULL) {
    // if the type is not directory just print
    if (dir->d_type != DT_DIR) {
      memset(&user_t, 0, sizeof(user_t));
      sprintf(user_t.pathname, "%s%s", path, dir->d_name);
      user_t.permission = permission;
      log_debug("%s%s\n", path, dir->d_name);
      // user_t.type = MYUSER;
      // sendstruct(&user_t);
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

static void process_pair(json_value *pair) {
  if (pair->type != json_object) {
    log_err("parse pair failed");
    exit(-1);
  }

  memset(user_t.pathname, 0, sizeof(user_t.pathname));

  for (int i = 0; i < pair->u.array.length; i++) {
    if (!strcmp(pair->u.object.values[i].name, "PATH")) {
      strncpy(user_t.pathname, pair->u.object.values[i].value->u.string.ptr,
              MAX_PATH - 1);
    } else if (!strcmp(pair->u.object.values[i].name, "AUTHORITY")) {
      user_t.permission =
          get_mode_value(pair->u.object.values[i].value->u.string.ptr);
    }
  }

  if (user_t.pathname[strlen(user_t.pathname) - 1] == '/') {
    // dir
    get_dir_content(user_t.pathname, user_t.permission);
  } else {
    printf("path: %s, permission : %d\n", user_t.pathname, user_t.permission);
    sendstruct(&user_t);
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

void json_process(char *file_contents, int file_size) {
  json_char *json;
  json_value *value;
  json = (json_char *)file_contents;
  value = json_parse(json, file_size);

  if (value == NULL) {
    log_err("Unable to parse config_file!");
    free(file_contents);
    exit(1);
  }

  if (value->type != json_object) {
    log_err("parse root failed");
    exit(-1);
  }

  for (int i = 0; i < value->u.object.length; i++) {
    if (strstr(value->u.object.values[i].name, target)) {
      process_array(value->u.object.values[i].value);
    }
  }

  return;
}

int netlink_init(void) {
  sock_fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_USER);
  if (sock_fd < 0)
    return -1;

  memset(&src_addr, 0, sizeof(src_addr));
  src_addr.nl_family = AF_NETLINK;
  src_addr.nl_pid = getpid(); /* self pid */

  bind(sock_fd, (struct sockaddr *)&src_addr, sizeof(src_addr));

  memset(&dest_addr, 0, sizeof(dest_addr));
  memset(&dest_addr, 0, sizeof(dest_addr));
  dest_addr.nl_family = AF_NETLINK;
  dest_addr.nl_pid = 0;    /* For Linux Kernel */
  dest_addr.nl_groups = 0; /* unicast */

  nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD));
  memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));
  nlh->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD);
  nlh->nlmsg_pid = getpid();
  nlh->nlmsg_flags = 0;

  return 0;
}

int main(int argc, char **argv) {
  if (netlink_init() == -1) {
    printf("netlink init failed\n");
    exit(-1);
  }
  int ch;
  printf("--------------------------\n");
  while ((ch = getopt(argc, argv, "rht:c:")) != -1) {
    switch (ch) {
    case 'r': {
      user_t.type = CMD;
      strcpy(user_t.pathname, "clear");
      sendcmd(&user_t);
      close(sock_fd);
      return 0;
    }
    case 'h':
      printf("HAVE option: -h\n");
      break;
    case 'c': {
      strcpy(config_path, optarg);
      break;
    }
    case 't': {
      user_t.type = CMD;
      sprintf(target, "%s", optarg);
      sprintf(user_t.pathname, "tar-%s", optarg);
      sendcmd(&user_t);
      close(sock_fd);
      break;
    }
    case '?':
      printf("Unknown option: %c\n", (char)optopt);
      break;
    }
  }

  int file_size;
  char *file_contents = getfile_content(&file_size);
  json_process(file_contents, file_size);
  close(sock_fd);
  return 0;

  // printf("Waiting for message from kernel\n");

  // /* Read message from kernel */
  // recvmsg(sock_fd, &msg, 0);
  // printf("Received message payload: %s\n", (char *)NLMSG_DATA(nlh));
  // close(sock_fd);
}

void sendstruct(struct myuser *u) {
  u->type = MYUSER;
  memcpy(NLMSG_DATA(nlh), (void *)u, sizeof(*u));
  iov.iov_base = (void *)nlh;
  iov.iov_len = nlh->nlmsg_len;
  msg.msg_name = (void *)&dest_addr;
  msg.msg_namelen = sizeof(dest_addr);
  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;
  printf("Sending struct to kernel: %s \n", u->pathname);
  sendmsg(sock_fd, &msg, 0);
}

void sendcmd(struct myuser *u) {
  u->type = CMD;
  memcpy(NLMSG_DATA(nlh), (void *)u, sizeof(*u));
  iov.iov_base = (void *)nlh;
  iov.iov_len = nlh->nlmsg_len;
  msg.msg_name = (void *)&dest_addr;
  msg.msg_namelen = sizeof(dest_addr);
  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;
  printf("Sending cmd %s to kernel\n", u->pathname);
  sendmsg(sock_fd, &msg, 0);
}