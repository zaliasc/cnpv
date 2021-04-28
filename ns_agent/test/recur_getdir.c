#include <dirent.h>
#include <stdio.h>
#include <string.h>

void show_dir_content(char *path) {
  DIR *d = opendir(path);
  if (d == NULL)
    return;
  struct dirent *dir;
  while ((dir = readdir(d)) != NULL) {
    // if the type is not directory just print
    if (dir->d_type != DT_DIR)
      printf("%s/%s\n", path, dir->d_name);
    else if (dir->d_type == DT_DIR && strcmp(dir->d_name, ".") != 0 &&
             strcmp(dir->d_name, "..") != 0) {
      // if it is a directory
      char d_path[255];
      sprintf(d_path, "%s/%s", path, dir->d_name);
      show_dir_content(d_path);
    }
  }
  closedir(d);
}

int main(int argc, char **argv) {

  show_dir_content(argv[1]);

  return (0);
}