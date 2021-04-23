#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include "types.h"

extern int log_fd;

#ifdef DEBUG_STDOUT
    #define DEBUG(format, ...) fprintf (stdout, format, ##__VA_ARGS__) 
#else
#ifdef DEBUG_FILE
    #define DEBUG(format, ...) dprintf (log_fd, format, ##__VA_ARGS__)
#else
    #define DEBUG(format, ...)
#endif
#endif

#ifdef LOG_FATAL
#define log_fatal(format, ...) \
    do { \
        DEBUG("\n->FATAL @ FUNC:%s FILE:%s LINE:%d \n" format "\n",\
                    __func__, __FILE__, __LINE__, ##__VA_ARGS__ );\
    } while (0)
#else
#define log_fatal(format, ...)
#endif

#ifdef LOG_ERR
#define log_err(format,...) \
    do { \
        DEBUG("\n->ERR   @ FUNC:%s FILE:%s LINE:%d \n" format "\n",\
                    __func__, __FILE__, __LINE__, ##__VA_ARGS__ );\
    } while (0)
#else
#define log_err(format,...)
#endif

#ifdef LOG_WARN
#define log_warn(format, ...) \
    do { \
        DEBUG("\n->WARN  @ FUNC:%s \n" format "\n",__func__, ##__VA_ARGS__ );\
    } while (0)
#else
#define log_warn(format, ...)
#endif

#ifdef LOG_INFO
#define log_info(format, ...) \
    do { \
        DEBUG("\n->INFO  \n"format"\n",##__VA_ARGS__ );\
    } while (0)
#else
#define log_info(format, ...)
#endif


#ifdef LOG_DEBUG
#define log_debug(format, ...) \
    do { \
        DEBUG("\n->DEBUG \n"format"\n",##__VA_ARGS__ );\
    } while (0)
#else
#define log_debug(format, ...)
#endif

#endif // LOG_H