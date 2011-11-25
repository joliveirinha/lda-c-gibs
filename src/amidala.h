#ifndef AMIDALA_H
#define AMIDALA_H

#include "lda.h"
#include "corpus.h"

#define LDA_MODEL 1

#define MAX(a, b) (a > b ? a : b )

lda_model_t* amidala_lda(int k, double alpha, double beta, 
                         int max_iter, int interval, double convergence,
                         corpus_t *c);


#endif
