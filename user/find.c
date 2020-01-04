#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char* fmtname(char *path) {
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
  return buf;
}

int match(char *text, char *pattern) {
  if (*pattern == '\0' || *pattern == ' ') {
    if (*text == '\0' || *text == ' ') return 0;
    return -1;
  }
  int firstmatch = text && (*pattern == *text || *pattern == '.');
  if (strlen(pattern) >= 2 && *(pattern+1) == '*') {
    if (match(text, pattern+2) == 0 || (firstmatch && match(text+1, pattern) == 0)) {
      return 0;
    }
    return -1;
  } else {
    if (firstmatch && match(text+1, pattern+1) == 0) {
      return 0;
    }
    return -1;
  }
}

void find(char *path, char *pattern) {
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if ((fd = open(path, 0)) < 0) {
    printf("find: cannot open %s\n", path);
    return;
  }

  if (fstat(fd, &st) < 0) {
    printf("find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch (st.type) {
  case T_FILE:
    if (match(fmtname(path), pattern) == 0) {
      printf("%s\n", path);
    }
    break;

  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf("find: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';
    while (read(fd, &de, sizeof(de)) == sizeof(de)) {
      if (de.inum == 0 || strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      if (stat(buf, &st) < 0) {
        printf("find: cannot stat %s\n", buf);
        continue;
      }
      find(buf, pattern);
    }
    break;
  }
  close(fd);
}

int main(int argc, char *argv[]) {
  if (argc < 3) {
    printf("find: not enough arguments\n");
    exit();
  }

  find(argv[1], argv[2]);
  exit();
}