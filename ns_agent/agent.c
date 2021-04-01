#include "agent.h"

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

struct real_calls real_call;

char * config_path;

static char * log_path = "/tmp/cnpv.log";

int log_fd;

// static int thread_num;

// tpool_t *pool;

static pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

// static void thread_init(int thread_num) {
//   if (create_tpool(&pool, thread_num) != 0) {
//     log_err("create tpool failed!\n");
//     exit(-1);
//   }
//   log_info("create tpool success! thread_num = %d", thread_num);
// }

static void getenv_options()
{
  config_path = getenv("CNPV_PATH");
  // const char* thread_num_p = getenv("CNPV_THREAD");
  if (!config_path) {
    log_err("please check env: CNPV_PATH");
    exit(-1);
  }
  // thread_num = atoi(thread_num_p);
  return;
}

static void init_preload()
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

  real_call.real_open = dlsym(RTLD_NEXT, "open");
  real_call.real_fopen = dlsym(RTLD_NEXT, "fopen");
  real_call.real_dprintf = dlsym(RTLD_NEXT, "dprintf");
  real_call.real_printf = dlsym(RTLD_NEXT, "printf");
  real_call.real_openat = dlsym(RTLD_NEXT, "openat");

  log_fd = real_call.real_open(log_path, O_RDWR|O_CREAT|O_APPEND, S_IRWXU);

  if (log_fd == 0) {
    exit (-1);
  }

  getenv_options();
  config_init();
  // thread_init(thread_num);
  init = 1;

out:
  pthread_mutex_unlock(&mut);
}

static int handle_request(void * data) {
  // log_info("handle!");
  struct handle_args * args = (struct handle_args *)data;
  if (check_permission(args->pathname, args->flags) == true) {
    if (args->mode == 0)
      return real_call.real_open(args->pathname, args->flags);
    else
      return real_call.real_open(args->pathname, args->flags, args->mode);
  }
  else {
    errno = 134;
    return -1;
  }
}

int open(const char *pathname, int flags, ...) {
  init_preload();
  log_err("start open(): %s ", pathname);
  struct handle_args data = {.pathname = pathname, .flags = flags, .type = OPEN};
  if(flags & O_CREAT) {
    va_list v;
    va_start(v, flags);
    data.mode = va_arg(v, mode_t);
  }
  else
    data.mode = 0;
  log_err("open(): %s ", pathname);
  return real_call.real_open(pathname, flags, data.mode); 
  // add_task_2_tpool(pool, handle_request, &data);

  // return handle_request(&data);
}

int openat(int dirfd, const char *pathname, int flags, ...) {
  init_preload(); 
  log_err("start openat(): %s ", pathname);
  struct handle_args data = {.pathname = pathname, .flags = flags, .type = OPENAT};
  if(flags & O_CREAT) {
    va_list v;
    va_start(v, flags);
    data.mode = va_arg(v, mode_t);
  }
  else
    data.mode = 0;
  log_err("openat(): %s ", pathname);
  return real_call.real_openat(dirfd, pathname, flags, data.mode);
}

FILE *fopen(const char *pathname, const char *mode) {
  init_preload();
  log_err("start fopen(): %s ", pathname);
  log_err("fopen(): %s ", pathname);
  return real_call.real_fopen(pathname, mode);
}