#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
  int ticks, r;

  if(argc != 2)
  {
    fprintf(2, "Usage: sleep ticks\n");
    exit(1);
  }

  ticks = atoi(argv[1]);
  r = sleep(ticks);

  exit(!!r);
}
