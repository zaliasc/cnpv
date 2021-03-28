#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dlfcn.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include <stdarg.h>

#include "log.h"
#include "types.h"

#define USE_GLOBAL_LOCK 1

#ifdef USE_GLOBAL_LOCK
#define RET_W_UNLOCK(lock_name, ret_val) do {           \
        pthread_mutex_unlock(ADD_REF(lock_name));       \
        return ret_val;                                 \
    } while (0)
#else
#define RET_W_UNLOCK(lock_name, ret_val) do {           \
        return ret_val;                                 \
    } while (0)
#endif

#ifdef USE_GLOBAL_LOCK
static pthread_mutex_t giant_lock = PTHREAD_MUTEX_INITIALIZER;
#define GLOBAL_LOCK do {                        \
        pthread_mutex_lock(&giant_lock);        \
    } while (0)
#define GLOBAL_UNLOCK do {                      \
        pthread_mutex_unlock(&giant_lock);      \
    } while (0)
#else
#define GLOBAL_LOCK do {} while (0)
#define GLOBAL_UNLOCK do {} while (0)
#endif

typedef int (*f_open)(int domain, int type, int protocol);

struct open_calls
{
  int (*open_call)(const char *pathname, int flags, ...);
};

static struct open_calls real_open;

char * config_path;

static int thread_num;

static pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

extern bool check_permission();

void getenv_options(void)
{
  config_path = getenv("CNPV_PATH");
  const char* thread_num_p = getenv("CNPV_THREAD");
  if (!config_path || !thread_num_p) {
    log_err("please check env: CNPV_PATH, CNPV_THREAD");
    exit(-1);
  }
  thread_num = atoi(thread_num_p);
  return;
}

static void init_preload(void)
{
  static int init;
  // quick check without lock
  if (init) {
      return;
  }

  pthread_mutex_lock(&mut);
  if (init) {
      goto out;
  }

  real_open.open_call = dlsym(RTLD_NEXT, "open");
  getenv_options();
  init = 1;

out:
  pthread_mutex_unlock(&mut);
}

int open(const char *path, int oflag, ...) {
  init_preload();
  struct open_thread_args data;
  data.pathname = path;
  data.flags = oflag;
  if(oflag & O_CREAT) {
    va_list v;
    va_start(v, oflag);
    data.mode = va_arg(v, mode_t);
  }
  else
    data.mode = 0;
  return handle_request(&data);
}

int handle_request(void * data) {
  log_info("handle!");
  if (check_permission() == true) {
    struct open_thread_args * args = (struct open_thread_args *)data;
    if (args->mode == 0)
      return real_open.open_call(args->pathname, args->flags);
    else
      return real_open.open_call(args->pathname, args->flags, args->mode);
  }
  else {
    errno = 134;
    return -1;
  }
}