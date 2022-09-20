#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void feed(int fd, int i);
void generate(int fd, int max);
void sieve(int fd);

#define GENERATE_MAX 35

int
main(int argc, char *argv[])
{
  int fd[2];
  int pid;

  if(pipe(fd) < 0)
    exit(1);
  if((pid = fork()) < 0)
    exit(1);
  if(pid != 0)  /* parent */
  {
    close(fd[0]);
    generate(fd[1], GENERATE_MAX);
    close(fd[1]);
    if(wait(0) < 0)
      exit(1);
  }
  else  /* child */
  {
    close(fd[1]);
    sieve(fd[0]);
    close(fd[0]);
  }
  exit(0);
}

void
generate(int fd, int max)
{
  int i;

  if(max < 2)
    exit(-1);
  for(i = 2; i <= max; ++i)
    feed(fd, i);
}

void
feed(int fd, int i)
{
  if(write(fd, &i, sizeof i) != (int)sizeof i)
    exit(1);
}

void
sieve(int fd)
{
  int fd_new[2];
  int pid = 0;
  int p, i, r;

  if((read(fd, &p, sizeof p)) != (int)sizeof p)
    exit(-1);
  printf("prime %d\n", p);

  while((r = read(fd, &i, sizeof i)) == (int)sizeof i)
  {
    if(i % p == 0)
      continue;
    if(pid == 0)
    {
      if(pipe(fd_new) < 0)
        exit(-1);
      if((pid = fork()) < 0)
        exit(-1);
      if(pid != 0)  /* parent */
        close(fd_new[0]);
      else  /* child */
      {
        close(fd_new[1]);
        sieve(fd_new[0]);
        close(fd_new[0]);
        exit(0);
      }
    }
    feed(fd_new[1], i);
  }
  if(r != 0)
    exit(1);
  close(fd_new[1]);
  if(wait(0) < 0)
    exit(1);
}
