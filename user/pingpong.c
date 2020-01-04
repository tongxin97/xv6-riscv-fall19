#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
  char buf[1];
  int parent_fds[2], child_fds[2];
  pipe(parent_fds);
  pipe(child_fds);

  int pid = fork();
  if (pid != 0) { // parent
    write(parent_fds[1], "\n", 1);
    wait();
    if (read(child_fds[0], buf, sizeof(buf)) == 1) {
      printf("%d: received pong\n", getpid());
    }
  } else {
    if (read(parent_fds[0], buf, sizeof(buf)) == 1) {
      printf("%d: received ping\n", getpid());
    }
    write(child_fds[1], "\n", 1);
  }
  exit();
}