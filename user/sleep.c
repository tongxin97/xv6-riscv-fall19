#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("sleep error: user should enter # of ticks\n");
    exit();
  }
  int n = atoi(argv[0]);
  if (sleep(n) < 0) {
    printf("sleep error\n");
  }
  exit();
}