#include <stdio.h>
#include <stdlib.h>

#include "utils.h"


void die(const char *message)
{
  fprintf(stderr, "%s\n", message);
  exit(1);
}

