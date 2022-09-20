#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

char *basename(const char *path);
void find(const char *path, const char *name);

int
main(int argc, const char *argv[])
{
  if(argc != 3)
  {
    fprintf(2, "usage: find path name\n");
    exit(1);
  }
  find(argv[1], argv[2]);
  exit(0);
}

void
find(const char *path, const char *name)
{
  char buf[512];
  int fd;
  struct dirent dent;
  struct stat sbuf;
  const char *bsname;
  int r, pathlen;

  if((fd = open(path, O_RDONLY)) < 0)
  {
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }
  if(fstat(fd, &sbuf) < 0)
  {
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  bsname = basename(path);
  if(!strcmp(bsname, name))
    printf("%s\n", path);

  if(sbuf.type == T_DIR)
  {
    pathlen = strlen(path);
    if(pathlen + DIRSIZ + 2 > sizeof buf)
      fprintf(2, "find: skipping too long directory: %s\n", path);
    else
    {
      strcpy(buf, path);
      buf[pathlen++] = '/';
      while((r = read(fd, &dent, sizeof dent)) == (int)sizeof dent)
      {
        if(dent.inum == 0)
          continue;
        if(!strcmp(dent.name, "."))
          continue;
        if(!strcmp(dent.name, ".."))
          continue;
        strcpy(&buf[pathlen], dent.name);
        find(buf, name);
      }
      if(r != 0)
        fprintf(2, "find: error reading %s\n", path);
    }
  }
  close(fd);
}

char *
basename(const char *path)
{
  const char *ptr = path, *p;
  while(1)
  {
    p = strchr(ptr, '/');
    if(p == 0)
      return (char *)ptr;
    ptr = p + 1;
  }
}
