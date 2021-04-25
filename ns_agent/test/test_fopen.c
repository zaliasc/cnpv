#include <stdio.h>

int main(int argc, char **argv) {
  FILE *ptr = fopen("/etc/hosts", "r");
  fclose(ptr);
  return 0;
}