
#include "cokus.h"
#include "distributions.h"

#define myrand() ((double) ( randomMT() / 4294967296.))

double random_uniform()
{
  return myrand();
}

int random_multinomial(double *p, int n, double sum)
{
  int i;
  double r = random_uniform();
  
  for (i=0;i<n;i++)
  {
    r -= p[i]/sum;
    if (r<=0)
      break;
  }

  return i;
}

