#ifndef LOG_H
#define LOG_H

#define __DEBUG

#ifdef __DEBUG
    #define DEBUG(format, ...) printf (format, ##__VA_ARGS__)
#else
    #define DEBUG(format, ...)
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