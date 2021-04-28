#ifndef TYPES_H
#define TYPES_H

#include <pthread.h>
#include <sys/types.h>

// logs
#define LOG_INFO
#define LOG_DEBUG
#define LOG_WARN
#define LOG_ERR
#define LOG_FATAL

#define DEBUG_FILE
// #define DEBUG_STDOUT

// cache
#define USE_CACHE

/* permission define. */
#define O_ALLOW 01000
#define O_READ 00100
#define O_WRITE 00010
#define O_FORBIDDEN 00001

#define FOPEN_MODE_FLAG 0xFFFF

// #define O_EXEC 0001

#define MAX_PATH 255

// typedef FILE *(*fopen_t)(const char *pathname, const char *mode);
// typedef int (*open_t)(const char *pathname, int flags, ...);
// typedef int (*open64_t)(const char *pathname, int flags, ...);
// typedef int (*openat_t)(int dirfd, const char *pathname, int flags, ...);
// typedef int (*openat64_t)(int dirfd, const char *pathname, int flags, ...);

// struct real_calls {
//     fopen_t real_fopen;
//     open_t real_open;
//     open64_t real_open64;
//     openat_t real_openat;
//     openat64_t real_openat64;
// };

// typedef enum {
//     OPEN,
//     OPEN64,
//     OPENAT,
//     OPENAT64,
//     FOPEN,
// } FUNC_TYPE;

// struct handle_args {
//     FUNC_TYPE type;
//     const char *pathname;
//     int flags;
//     mode_t mode;
//     int dirfd;
//     const char *f_mode;
// };

struct user {
    char pathname[MAX_PATH];
    int permission;
};

#endif /* TYPES_H */
