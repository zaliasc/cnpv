#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

static char short_option[] = "c:t:";

static struct option long_options[] = {
  {"config",  required_argument, 0,  'c' },
  {"thread",  required_argument, 0,  't' },
  {0,         0,                 0,  0 }
};

static char path[1000];

static int thread_num;

int main1(int argc, char ** argv) {

  int opt;
  extern char* optarg;
  extern int optind;
  extern int optopt;
  extern int opterr;

  while ((opt = getopt_long(argc, argv, short_option, long_options, NULL)) != -1) {
    switch (opt) {
      case 'c' :
        fprintf(stdout, "option c with value '%s'\n", optarg);
        strcpy(path, optarg);
        break;
      case 't' :
        fprintf(stdout, "option t with value '%s'\n", optarg);
        thread_num = atoi(optarg);
        break;
      case '?':
        break;
        exit(-1);
      default:
        fprintf(stdout, "getopt returned character code 0%o ??\n", opt);
    }
  }

  exit(EXIT_SUCCESS);
}

void getenv_options(void)
{
  const char* config_path = getenv("CNPV_PATH");
  const char* thread_num = getenv("THREAD_NUM");
  if (config_path && thread_num) {
    printf("%s %d\n", config_path, atoi(thread_num));
  }
  else {
    printf("failed");
  }
    return;
}

int main() {
  getenv_options();
  return 0;
}