#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void prime(int fds[]) {
  close(fds[1]);
  int p;
  if (read(fds[0], &p, sizeof(p)) <= 0) {
    return;
  }
  printf("prime %d\n", p);

  int nxt_fds[2];
  pipe(nxt_fds);

  if (fork() != 0) {
    close(nxt_fds[0]);
    int n;
    while (read(fds[0], &n, sizeof(n)) > 0) {
      if (n % p != 0) {
        write(nxt_fds[1], &n, sizeof(n));
      }
    }
    close(fds[0]);
    close(nxt_fds[1]);
    wait();
  } else {
    prime(nxt_fds);
  }
}

int main(int argc, char *argv[]) {
  int fds[2];
  pipe(fds);

  if (fork() != 0) {
    close(fds[0]);
    int first = 2, last = 35;
    for (int i = first; i <= last; i++) {
      write(fds[1], &i, sizeof(i));
    }
    close(fds[1]);
    wait();
  } else {
    prime(fds);
  }

  exit();
}