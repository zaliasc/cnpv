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

  ret = kernel_read(file, data, size, &offset);

  set_fs(oldfs);
  return ret;
}

int file_write(struct file *file, unsigned long long offset,
               unsigned char *data, unsigned int size) {
  mm_segment_t oldfs;
  int ret;

  oldfs = get_fs();
  set_fs(get_ds());

  ret = kernel_write(file, data, size, &offset);

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

char *load_file(char *filename, int *filesize) {
  *filesize = file_size(filename);
  int size = *filesize;
  int record = 0;
  char *buf = kmalloc(size, GFP_KERNEL);
  memset(buf, 0, size);
  struct file *f = file_open(filename, O_RDONLY, 0);
  file_read(f, 0, buf, size); 

  // while (size > 0) {
  //   if (size >= 512) {
  //     file_read(f, record, buf + record, 512);
  //     size -= 512;
  //     record += 512;
  //   } else {
  //     file_read(f, record, buf + record, size);
  //   }
  //   printk("size: %d record: %d", size, record);
  // }
  printk("size : %d \nbuf : %s",size, buf);
  file_close(f);
  return buf;
}

// static void read_file(char *filename) {
//   struct file *f = NULL;
//   int fd;
//   char buf[1];
//   loff_t f_pos = 0;
//   mm_segment_t old_fs = get_fs();
//   tab = kmalloc(15, GFP_KERNEL);
//   set_fs(KERNEL_DS);

//   f = filp_open(filename, O_RDONLY, 0);
//   if (!IS_ERR(f)) {
//     printk(KERN_DEBUG);
//     while (kernel_read(f, buf, 1, &f_pos) == 1) {
//       printk("%c", buf[0]);
//       if (buf[0] != '\n') {
//         tab[i] = buf[0];
//         printk("tab[%d] = %c", i, tab[i]);
//         i++;
//       }
//     }
//     printk("\n");
//     filp_close(f, NULL);
//   }
//   set_fs(old_fs);
// }