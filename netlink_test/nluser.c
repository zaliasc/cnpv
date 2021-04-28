#include "json.h"
#include "log.h"
#include <fcntl.h>
#include <linux/netlink.h>
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

char config_path[] = "/home/zhuzhicheng/project/cnpv/ns_agent/config.json";

char *getfile_content() {
  int fd;
  struct stat filestatus;
  int file_size;
  char *file_contents;

  if (stat(config_path, &filestatus) != 0) {
    printf("File %s not found", config_path);
    exit(-1);
  }

  file_size = filestatus.st_size;
  file_contents = (char *)malloc(filestatus.st_size);
  if (file_contents == NULL) {
    printf("Memory error: unable to allocate %d bytes", file_size);
    exit(-1);
  }

  fd = open(config_path, O_RDONLY);
  if (fd == 0) {
    free(file_contents);
    exit(-1);
  }
  if (read(fd, file_contents, file_size) == -1) {
    printf("read failed");
    free(file_contents);
    exit(-1);
  }
  close(fd);

  printf("file_content: %s", file_contents);
}

// static void process_pair(json_value *pair) {
//   if (pair->type != json_object) {
//     log_err("parse pair failed");
//     exit(-1);
//   }

//   struct user tmp = {.pathname = {0}};

//   for (int i = 0; i < pair->u.array.length; i++) {
//     if (!strcmp(pair->u.object.values[i].name, "PATH")) {
//       strncpy(tmp.pathname, pair->u.object.values[i].value->u.string.ptr,
//               MAX_PATH - 1);
//     } else if (!strcmp(pair->u.object.values[i].name, "AUTHORITY")) {
//       tmp.permission =
//           get_mode_value(pair->u.object.values[i].value->u.string.ptr);
//     }
//   }

//   if (tmp.pathname[strlen(tmp.pathname) - 1] == '/') {
//     // dir
//     get_dir_content(tmp.pathname, tmp.permission);
//   } else {
//     // hashmap_set(map, &tmp);
//   }
// }

// static void process_array(json_value *array) {
//   if (array->type != json_array) {
//     log_err("parse array failed");
//     exit(-1);
//   }

//   for (int i = 0; i < array->u.array.length; i++) {
//     process_pair(array->u.array.values[i]);
//   }
// }

// void json_process(char *file_contents, int file_size) {
//   json_char *json;
//   json_value *value;
//   json = (json_char *)file_contents;
//   value = json_parse(json, file_size);

//   if (value == NULL) {
//     log_err("Unable to parse config_file!");
//     free(file_contents);
//     exit(1);
//   }

//   if (value->type != json_object) {
//     log_err("parse root failed");
//     exit(-1);
//   }

//   for (int i = 0; i < value->u.object.length; i++) {
//     // if (strstr(value->u.object.values[i].name,
//     // program_invocation_short_name)) {
//     process_array(value->u.object.values[i].value);
//     // }
//   }

//   return;
// }

void mysendmsg(const char *str);

int main() {
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

  char *file_contents = getfile_content();

  // strcpy(NLMSG_DATA(nlh), file_contents);

  // iov.iov_base = (void *)nlh;
  // iov.iov_len = nlh->nlmsg_len;
  // msg.msg_name = (void *)&dest_addr;
  // msg.msg_namelen = sizeof(dest_addr);
  // msg.msg_iov = &iov;
  // msg.msg_iovlen = 1;

  // printf("Sending message to kernel\n");
  // sendmsg(sock_fd, &msg, 0);
  mysendmsg("111");
  mysendmsg("222");
  mysendmsg("333");
  mysendmsg("444");

  printf("Waiting for message from kernel\n");

  /* Read message from kernel */
  recvmsg(sock_fd, &msg, 0);
  printf("Received message payload: %s\n", (char *)NLMSG_DATA(nlh));
  close(sock_fd);
}

void mysendmsg(const char *str) {
  strcpy(NLMSG_DATA(nlh), str);
  iov.iov_base = (void *)nlh;
  iov.iov_len = nlh->nlmsg_len;
  msg.msg_name = (void *)&dest_addr;
  msg.msg_namelen = sizeof(dest_addr);
  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;
  printf("Sending message to kernel\n");
  sendmsg(sock_fd, &msg, 0);
}