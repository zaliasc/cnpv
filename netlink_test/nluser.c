#include <fcntl.h>
#include <linux/netlink.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

#define NETLINK_USER 31

#define MAX_PAYLOAD 1024 * 16 /* maximum payload size*/
struct sockaddr_nl src_addr, dest_addr;
struct nlmsghdr *nlh = NULL;
struct iovec iov;
int sock_fd;
struct msghdr msg;

char config_path[] = "/home/zhuzhicheng/project/cnpv/ns_agent/config.json";

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

  strcpy(NLMSG_DATA(nlh), file_contents);

  iov.iov_base = (void *)nlh;
  iov.iov_len = nlh->nlmsg_len;
  msg.msg_name = (void *)&dest_addr;
  msg.msg_namelen = sizeof(dest_addr);
  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;

  printf("Sending message to kernel\n");
  sendmsg(sock_fd, &msg, 0);
  printf("Waiting for message from kernel\n");

  /* Read message from kernel */
  recvmsg(sock_fd, &msg, 0);
  printf("Received message payload: %s\n", (char *)NLMSG_DATA(nlh));
  close(sock_fd);
}