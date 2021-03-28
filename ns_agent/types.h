#ifndef TYPES_H
#define TYPES_H

#include <pthread.h>
#include <sys/types.h>

#define O_CREAT 0100

#define O_READ 0100
#define O_WRITE 0010
#define O_EXEC 0001



struct open_thread_args {
        char * pathname;
        int flags;
        mode_t mode;
};

#endif /* TYPES_H */
