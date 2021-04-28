#ifndef TYPES_H
#define TYPES_H

/* permission define. */
#define O_ALLOW 01000
#define O_READ 00100
#define O_WRITE 00010
#define O_FORBIDDEN 00001

#define FOPEN_MODE_FLAG 0xFFFF

#define MAX_PATH 255

typedef enum {
    FALSE,
    TRUE
} bool;


typedef enum {
    CMD,
    STR,
    MYUSER
} netlink_type;

struct myuser {
  netlink_type type;
  char pathname[MAX_PATH];
  int permission;
};

struct mapuser {
  char pathname[MAX_PATH];
  int permission;
};

#endif /* TYPES_H */
