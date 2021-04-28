#ifndef UTIL_H
#define UTIL_H

#include <asm/segment.h>
#include <linux/buffer_head.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/stat.h>
#include <linux/uaccess.h>
#include <linux/string.h>

struct file *file_open(const char *path, int flags, int rights);
void file_close(struct file *file);
int file_read(struct file *file, unsigned long long offset, unsigned char *data, unsigned int size);
int file_write(struct file *file, unsigned long long offset, unsigned char *data, unsigned int size);
int file_sync(struct file *file);
char *load_file(char *filename, int *filesize);
#endif  // UTILL_H