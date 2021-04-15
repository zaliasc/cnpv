#ifndef LOG_H
#define LOG_H

#include <stdio.h>
extern int log_fd;

// #define __DEBUG_STDOUT
// #define __DEBUG_FILE

#ifdef __DEBUG_STDOUT
    // #define DEBUG(format, ...) real_call.real_printf (format, ##__VA_ARGS__)
    #define DEBUG(format, ...) fprintf (stdout, format, ##__VA_ARGS__) 
#else
#ifdef __DEBUG_FILE
    // #define DEBUG(format, ...) real_call.real_dprintf (log_fd, format, ##__VA_ARGS__)
    #define DEBUG(format, ...) dprintf (log_fd, format, ##__VA_ARGS__)
#else
    #define DEBUG(format, ...)
#endif
#endif

#define log_fatal(format, ...) \
    do { \
        DEBUG("\n->FATAL @ FUNC:%s FILE:%s LINE:%d \n" format "\n",\
                    __func__, __FILE__, __LINE__, ##__VA_ARGS__ );\
    } while (0)

#define log_err(format, ...) \
    do { \
        DEBUG("\n->ERR   @ FUNC:%s FILE:%s LINE:%d \n" format "\n",\
                    __func__, __FILE__, __LINE__, ##__VA_ARGS__ );\
    } while (0)

#define log_warn(format, ...) \
    do { \
        DEBUG("\n->WARN  @ FUNC:%s \n" format "\n",__func__, ##__VA_ARGS__ );\
    } while (0)

#define log_info(format, ...) \
    do { \
        DEBUG("\n->INFO  \n"format"\n",##__VA_ARGS__ );\
    } while (0)

#define log_debug(format, ...) \
    do { \
        DEBUG("\n->DEBUG \n"format"\n",##__VA_ARGS__ );\
    } while (0)

#endif