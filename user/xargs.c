#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

const char *spaces = " \f\n\r\t\v";

int isspace(int ch);
int isnulspace(int ch);
char *token_begin(const char *str);
char *token_end(const char *str);
void xargs(int argc, char *argv[], char *buf);
void run(char *argv[]);

int
main(int argc, char *argv[])
{
  char buf[512];
  while(*gets(buf, sizeof buf))
    xargs(argc - 1, &argv[1], buf);
  exit(0);
}

void
xargs(int argc0, char *argv0[], char *buf)
{
  int argc = 0;
  char *argv[MAXARG + 1];

  while(argc != argc0)
  {
    argv[argc] = argv0[argc];
    ++argc;
  }
  while(1)
  {
    buf = token_begin(buf);
    if(*buf == 0)
      break;
    if(argc == MAXARG)
    {
      fprintf(2, "xargs: too many arguments\n");
      exit(1);
    }
    argv[argc++] = buf;
    buf = token_end(buf);
    if(*buf == 0)
      break;
    *buf++ = 0;
  }
  argv[argc] = 0;
  run(argv);
}

void
run(char *argv[])
{
  int pid;

  if((pid = fork()) < 0)
  {
    fprintf(2, "xargs: fork failed\n");
    exit(1);
  }
  if(pid == 0)  /* child */
  {
    exec(argv[0], argv);
    fprintf(2, "xargs: exec %s failed\n", argv[0]);
    exit(1);
  }
  /* parent */
  if(wait(0) < 0)
  {
    fprintf(2, "xargs: wait failed\n");
    exit(1);
  }
}

int
isnulspace(int ch_in)
{
  char ch = (char)ch_in;
  return ch == 0 || strchr(spaces, ch);
}

int
isspace(int ch_in)
{
  char ch = (char)ch_in;
  return ch && strchr(spaces, ch);
}

char *
token_begin(const char *str)
{
  while(isspace(*str))
    ++str;
  return (char *)str;
}

char *
token_end(const char *str)
{
  while(isnulspace(*str) == 0)
    ++str;
  return (char *)str;
}
