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

char *load_file(char *filename, int *input_size) {
  struct kstat *stat;
  struct file *fp;
  mm_segment_t fs;
  loff_t pos = 0;
  char *buf;

  fp = filp_open(filename, O_RDWR, 0644);
  if (IS_ERR(fp)) {
    printk("Open file error!\n");
    return ERR_PTR(-ENOENT);
  }

  fs = get_fs();
  set_fs(KERNEL_DS);

  stat = (struct kstat *)kmalloc(sizeof(struct kstat), GFP_KERNEL);
  if (!stat)
    return ERR_PTR(-ENOMEM);

  vfs_stat(filename, stat);
  *input_size = stat->size;

  buf = kmalloc(*input_size, GFP_KERNEL);
  if (!buf) {
    kfree(stat);
    printk("malloc input buf error!\n");
    return ERR_PTR(-ENOMEM);
  }
  kernel_read(fp, buf, *input_size, &pos);

  filp_close(fp, NULL);
  set_fs(fs);
  kfree(stat);
  return buf;
}