#include <stdio.h>
#include <stdlib.h>

#include "utils.h"


/*
 * program behaviour utils functions
 */
void die(const char *message)
{
  fprintf(stderr, "%s\n", message);
  exit(1);
}

