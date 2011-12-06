#include "amidala.h"
#include "lda.h"

lda_model_t* amidala_lda(int k, double alpha, double beta, 
                         int max_iter, int interval, double convergence,
                         corpus_t *c) 
{
  lda_model_t *model = lda_create(k, c->num_terms, alpha, beta);
  
  lda_estimate(model, c, max_iter, interval, convergence);

  return model;
}
