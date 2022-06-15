#include "kernel/types.h"
#include "user/user.h"

int main(void)
{
  printf("\e[H\e[2J\e[3J");
  exit(0);
}
