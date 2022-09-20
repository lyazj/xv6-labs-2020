#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void ping(const int fd[2]);
void pong(const int fd[2]);

int
main(int argc, char *argv[])
{
  int pid;
  int ptoc[2], ctop[2];
  int fd[2];

  if(pipe(ptoc) < 0 || pipe(ctop) < 0)
    exit(1);
  if((pid = fork()) < 0)
    exit(1);
  if(pid != 0)  /* parent */
  {
    close(ptoc[0]);
    close(ctop[1]);
    fd[0] = ctop[0];
    fd[1] = ptoc[1];
    ping(fd);
    close(ptoc[1]);
    close(ctop[0]);
  }
  else  /* child */
  {
    close(ptoc[1]);
    close(ctop[0]);
    fd[0] = ptoc[0];
    fd[1] = ctop[1];
    pong(fd);
    close(ptoc[0]);
    close(ctop[1]);
  }
  exit(0);
}

void
ping(const int fd[2])
{
  char ch = 'p';

  if(write(fd[1], &ch, 1) != 1)
    exit(1);
  if(read(fd[0], &ch, 1) != 1)
    exit(1);
  printf("%d: received pong\n", getpid());
}

void
pong(const int fd[2])
{
  char ch;

  if(read(fd[0], &ch, 1) != 1)
    exit(1);
  printf("%d: received ping\n", getpid());
  if(write(fd[1], &ch, 1) != 1)
    exit(1);
}
