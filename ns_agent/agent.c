#include "agent.h"

#define USE_GLOBAL_LOCK 1

#ifdef USE_GLOBAL_LOCK
#define RET_W_UNLOCK(lock_name, ret_val)                                       \
  do {                                                                         \
    pthread_mutex_unlock(ADD_REF(lock_name));                                  \
    return ret_val;                                                            \
  } while (0)
#else
#define RET_W_UNLOCK(lock_name, ret_val)                                       \
  do {                                                                         \
    return ret_val;                                                            \
  } while (0)
#endif

#ifdef USE_GLOBAL_LOCK
static pthread_mutex_t giant_lock = PTHREAD_MUTEX_INITIALIZER;
#define GLOBAL_LOCK                                                            \
  do {                                                                         \
    pthread_mutex_lock(&giant_lock);                                           \
  } while (0)
#define GLOBAL_UNLOCK                                                          \
  do {                                                                         \
    pthread_mutex_unlock(&giant_lock);                                         \
  } while (0)
#else
#define GLOBAL_LOCK                                                            \
  do {                                                                         \
  } while (0)
#define GLOBAL_UNLOCK                                                          \
  do {                                                                         \
  } while (0)
#endif

struct real_calls real_call;

char *config_path;

char *cwd;

extern char *program_invocation_short_name;

// static char *log_path = "/tmp/cnpv.log";

int log_fd;

static pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

static void getenv_options() {
  config_path = getenv("CNPV_PATH");

  if (!config_path) {
    log_err("please check env: CNPV_PATH");
    exit(-1);
  }

  return;
}

static void init_preload() {
  static int init;
  // quick check without lock
  if (init) {
    return;
  }

  pthread_mutex_lock(&mut);
  if (init) {
    goto out;
  }

  real_call.real_fopen = dlsym(RTLD_NEXT, "fopen");
  real_call.real_open = dlsym(RTLD_NEXT, "open");
  real_call.real_open64 = dlsym(RTLD_NEXT, "open64");
  real_call.real_openat = dlsym(RTLD_NEXT, "openat");
  real_call.real_openat64 = dlsym(RTLD_NEXT, "openat64");

  cwd = getcwd(NULL, 0);

  if (cwd == NULL) {
    log_err("getcwd failed!");
    exit(-1);
  }

  char log_path[MAX_PATH];

  sprintf(log_path, "%s/%s-%s%s", cwd, "cnpv",program_invocation_short_name, ".log");

#ifdef DEBUG_FILE

  log_fd = real_call.real_open(log_path, O_RDWR | O_CREAT | O_APPEND, S_IRWXU);

  log_debug("log_fd : %d", log_fd);

  if (log_fd == -1) {
    fprintf(stdout, "open/create log file error");
    exit(-1);
  }
#endif  

  getenv_options();

  config_init();

#ifdef USE_CACHE
  cache_init();
#endif

  init = 1;

out:
  pthread_mutex_unlock(&mut);
}

static int handle_request(void *data) {
  struct handle_args *args = (struct handle_args *)data;

  if (check_permission(args->pathname, args->flags) == false) {
    log_info("check path %s failed!", args->pathname);
    errno = 134;
    return -1;
  }

  switch (args->type) {
  case OPEN: {
    log_info("handle open");
    if (__OPEN_NEEDS_MODE(args->flags))
      return real_call.real_open(args->pathname, args->flags, args->mode);
    else
      return real_call.real_open(args->pathname, args->flags);
  }
  case OPENAT: {
    log_info("handle openat");
    if (__OPEN_NEEDS_MODE(args->flags))
      return real_call.real_openat(args->dirfd, args->pathname, args->flags,
                                   args->mode);
    else
      return real_call.real_openat(args->dirfd, args->pathname, args->flags);
  }
  case OPEN64: {
    log_info("handle open64");
    if (__OPEN_NEEDS_MODE(args->flags))
      return real_call.real_open64(args->pathname, args->flags, args->mode);
    else
      return real_call.real_open64(args->pathname, args->flags);
  }
  case OPENAT64: {
    log_info("handle openat64");
    if (__OPEN_NEEDS_MODE(args->flags))
      return real_call.real_openat64(args->dirfd, args->pathname, args->flags,
                                     args->mode);
    else
      return real_call.real_openat64(args->dirfd, args->pathname, args->flags);
  }
  // case FOPEN: {
  //   log_info(level, "handle fopen");
  //   return real_call.real_fopen(args->pathname, args->f_mode);
  // }
  default: {
    log_err("undefined type!");
    errno = 134;
    return -1;
  }
  }
}

int open(const char *pathname, int flags, ...) {
  init_preload();
  log_info("call open(): %s ", pathname);
  struct handle_args data = {
      .pathname = pathname, .flags = flags, .type = OPEN};
  if (__OPEN_NEEDS_MODE(flags)) {
    va_list v;
    va_start(v, flags);
    data.mode = va_arg(v, mode_t);
  } else
    data.mode = 0;
  return handle_request(&data);
}

int open64(const char *pathname, int flags, ...) {
  init_preload();
  log_info("call open64(): %s ", pathname);
  struct handle_args data = {
      .pathname = pathname, .flags = flags, .type = OPEN64};
  if (__OPEN_NEEDS_MODE(flags)) {
    va_list v;
    va_start(v, flags);
    data.mode = va_arg(v, mode_t);
  } else
    data.mode = 0;
  return handle_request(&data);
}

int openat(int dirfd, const char *pathname, int flags, ...) {
  init_preload();
  log_info("call openat(): %s ", pathname);
  struct handle_args data = {
      .pathname = pathname, .flags = flags, .type = OPENAT, .dirfd = dirfd};
  if (__OPEN_NEEDS_MODE(flags)) {
    va_list v;
    va_start(v, flags);
    data.mode = va_arg(v, mode_t);
  } else
    data.mode = 0;
  return handle_request(&data);
}

int openat64(int dirfd, const char *pathname, int flags, ...) {
  init_preload();
  log_info("call openat(): %s ", pathname);
  struct handle_args data = {
      .pathname = pathname, .flags = flags, .type = OPENAT64, .dirfd = dirfd};
  if (__OPEN_NEEDS_MODE(flags)) {
    va_list v;
    va_start(v, flags);
    data.mode = va_arg(v, mode_t);
  } else
    data.mode = 0;
  return handle_request(&data);
}

FILE *fopen(const char *pathname, const char *mode) {
  init_preload();
  log_info("call fopen(): %s ", pathname);
  if (check_permission(pathname, FOPEN_MODE_FLAG) == false) {
    log_info("check path %s failed!", pathname);
    errno = 134;
    return NULL;
  }
  return real_call.real_fopen(pathname, mode);
  // struct handle_args data = {.pathname = pathname, .f_mode = mode, .type =
  // FOPEN}; return handle_request(&data);
}