#include "kernel/types.h"
#include "user/user.h"

int main(void)
{
  char msg = 'a';
  int i, r, forkpid, pid;
  int pipefd[2][2];
  char buf[16];

  for(i = 0; i < 2; ++i)
  {
    r = pipe(pipefd[i]);
    if(r < 0)
    {
      fprintf(2, "pingpong: pipe failed\n");
      exit(1);
    }
  }

  forkpid = fork();
  if(forkpid < 0)
  {
    fprintf(2, "pingpong: fork failed\n");
    exit(1);
  }

  pid = getpid();
  if(pid < 0)
  {
    fprintf(2, "pingpong: getpid failed\n");
    exit(1);
  }

  if(forkpid)  /* parent */
  {
    if(close(pipefd[0][0]) < 0 || close(pipefd[1][1]) < 0)
    {
      fprintf(2, "%d: close failed\n", pid);
      exit(1);
    }

    r = write(pipefd[0][1], &msg, 1);
    if(r <= 0)
    {
      fprintf(2, "%d: write failed\n", pid);
      exit(1);
    }

    r = read(pipefd[1][0], buf, 1);
    if(r <= 0)
    {
      fprintf(2, "%d: read failed\n", pid);
      exit(1);
    }

    printf("%d: received pong\n", pid);

    if(close(pipefd[0][1]) < 0 || close(pipefd[1][0]) < 0)
    {
      fprintf(2, "%d: close failed\n", pid);
      exit(1);
    }
  }
  else  /* child */
  {
    if(close(pipefd[0][1]) < 0 || close(pipefd[1][0]) < 0)
    {
      fprintf(2, "%d: close failed\n", pid);
      exit(1);
    }

    r = read(pipefd[0][0], buf, 1);
    if(r <= 0)
    {
      fprintf(2, "%d: read failed\n", pid);
      exit(1);
    }

    printf("%d: received ping\n", pid);

    r = write(pipefd[1][1], buf, 1);
    if(r <= 0)
    {
      fprintf(2, "%d: write failed\n", pid);
      exit(1);
    }

    if(close(pipefd[0][0]) < 0 || close(pipefd[1][1]) < 0)
    {
      fprintf(2, "%d: close failed\n", pid);
      exit(1);
    }
  }

  exit(0);
}
