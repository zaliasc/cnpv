#ifndef AGENT_H
#define AGENT_H

#ifndef _GNU_SOURCE 
#define _GNU_SOURCE
#endif

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

#include <fcntl.h>

#include "log.h"
#include "types.h"

int open(const char *path, int oflag, ...);

static int handle_request(void * data);

static void init_preload();

static void getenv_options();

extern void config_init();

extern bool check_permission(const char * pathname);

#endif