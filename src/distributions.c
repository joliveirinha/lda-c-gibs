
#include "cokus.h"
#include "distributions.h"

int random_multinomial(double *p, double sum)
{
  int i=0;
  double r = random_uniform() * sum;
  
  while (r>0.0)
    r -= p[i++];

  return i-1;
}

