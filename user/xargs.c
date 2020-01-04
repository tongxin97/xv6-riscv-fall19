#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
  if (argc < 2 || argc > MAXARG + 2) { // allowed # arguments: [0, MAXARG]
    printf("xargs error: invali number of arguments\n");
    exit();
  }

  char *pass[MAXARG];
  char buf[MAXARG][32];

  for (int i = 0; i < MAXARG; i++) {
    pass[i] = buf[i];
  }
  for (int i = 1; i < argc; i++) {
    strcpy(buf[i-1], argv[i]);
  }

  char input[32];
  int n;
  while ((n = read(0, input, sizeof(input))) > 0) {
    int i = argc-1;
    char *p = buf[i];
    for (char *q = input; *q; q++) {
      if (*q == ' ' || *q == '\n') {
        *p = '\0';
        p = buf[++i];
      } else {
        *p++ = *q;
      }
    }
    *p = '\0';
    pass[++i] = 0;

    if (fork()) {
      wait();
    } else {
      exec(pass[0], pass);
    }
  }

  if (n < 0) {
    printf("xargs error: read from input\n");
  }
  exit();
}
