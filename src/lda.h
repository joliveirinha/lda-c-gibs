#ifndef LDA_H
#define LDA_H

#include "corpus.h"

typedef struct _lda_suffstats_st {

} lda_suffstats_t;

typedef struct _lda_model_st {
  int k;
  double alpha;

  double **log_prob_w;
  int num_topics;
  int num_terms;

  lda_suffstats_t suffstats;
} lda_model_t;


lda_model_t* lda_create(int k, double alpha, corpus_t *c);
void lda_destroy(lda_model_t *model);
void lda_estimate(lda_model_t *model, corpus_t *corpus, int iterations);


#endif

