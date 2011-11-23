#include "amidala.h"
#include "lda.h"

lda_model_t* amidala_lda(int k, double alpha, int iterations, corpus_t *c) 
{
  lda_model_t *model = lda_create(k, alpha, c);
  
  lda_estimate(model, c, iterations);

  return model;
}
