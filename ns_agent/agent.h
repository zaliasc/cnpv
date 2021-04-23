#ifndef AGENT_H
#define AGENT_H

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

// #include <bits/types/FILE.h>
#include <dlfcn.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include "log.h"
#include "types.h"
#include "cache.h"

static int handle_request(void* data);

static void init_preload();

static void getenv_options();

extern void config_init();

extern bool check_permission(const char* pathname, int oflag);

#endif // AGENT_H