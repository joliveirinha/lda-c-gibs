#ifndef AMIDALA_H
#define AMIDALA_H

#include "lda.h"
#include "corpus.h"

#define LDA_MODEL 1

#define MAX(a, b) (a > b ? a : b )

lda_model_t* amidala_lda(int k, double alpha, int iterations, corpus_t *c);


#endif
