#include "utils.h"

struct file *file_open(const char *path, int flags, int rights) {
  struct file *filp = NULL;
  mm_segment_t oldfs;
  int err = 0;

  oldfs = get_fs();
  set_fs(get_ds());
  filp = filp_open(path, flags, rights);
  set_fs(oldfs);
  if (IS_ERR(filp)) {
    err = PTR_ERR(filp);
    return NULL;
  }
  return filp;
}

void file_close(struct file *file) { filp_close(file, NULL); }

int file_read(struct file *file, unsigned long long offset, unsigned char *data,
              unsigned int size) {
  mm_segment_t oldfs;
  int ret;

  oldfs = get_fs();
  set_fs(get_ds());

  ret = vfs_read(file, data, size, &offset);

  set_fs(oldfs);
  return ret;
}

int file_write(struct file *file, unsigned long long offset,
               unsigned char *data, unsigned int size) {
  mm_segment_t oldfs;
  int ret;

  oldfs = get_fs();
  set_fs(get_ds());

  ret = vfs_write(file, data, size, &offset);

  set_fs(oldfs);
  return ret;
}

int file_sync(struct file *file) {
  vfs_fsync(file, 0);
  return 0;
}

int file_size(const char *filename) {
  mm_segment_t oldfs;
  int ret;

  oldfs = get_fs();
  set_fs(get_ds());

  struct kstat stat;
  vfs_stat(filename, &stat);
  ret = (int)stat.size;

  set_fs(oldfs);
  return ret;
}

char *load_file(char *filename, int *file_size) {
  *filesize = file_size(filename);
  char *buf = kmalloc(filesize, GFP_KERNEL);
  struct file *f = file_open(filename, O_RDONLY, 0);
  file_read(f, buf, content, file_size);
  return buf;
}