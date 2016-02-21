#ifndef DISTRIBUTIONS_H
#define DISTRIBUTIONS_H

#include "cokus.h"

#define random_uniform() ((double) ( randomMT() / 4294967296.))

int random_multinomial(double *p, double sum);

#endif
