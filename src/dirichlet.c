#include <math.h>

#define HALF_LOG_TWO_PI 0.9189385332046726695409688545623794198036193

double log_gamma(double z)
{
  double result;
  int shift = 0;

  while (z<2) 
  {
    z++;
    shift++;
  }

  result = HALF_LOG_TWO_PI+(z-0.5)*log(z)-z+
           1/(12*z)-1/(360*z*z*z)+1/(1260*z*z*z*z*z);

  while (shift > 0)
  {
    shift--;
    z--;
    result -= log(z);
  }
  
  return result;
}


