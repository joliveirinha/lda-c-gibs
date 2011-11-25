
#include "cokus.h"
#include "distributions.h"

#define myrand() ((double) ( randomMT() / 4294967296.))

double random_uniform()
{
  return myrand();
}

int random_multinomial(double *p, int n)
{
  int i;
  double r = random_uniform();
  
  for (i=0;i<n;i++)
  {
    r -= p[i];
    if (r<=0)
      break;
  }

  return i;
}

