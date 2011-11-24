#ifndef LDA_H
#define LDA_H

#include "corpus.h"

typedef struct _lda_suffstats_st {
  int *word_topic;
  
} lda_suffstats_t;

typedef struct _lda_model_st {
  int k;
  double alpha;
  double beta;

  double **log_prob_w;
  int num_topics;
  int num_terms;
} lda_model_t;


lda_model_t* lda_create(int k, double alpha, double beta, corpus_t *c);
void lda_destroy(lda_model_t *model);
void lda_estimate(lda_model_t *model, corpus_t *corpus, int iterations);

lda_suffstats_t* lda_create_suffstats(lda_model_t *m, corpus_t *c);
void lda_destroy_suffstats(lda_suffstats_t *stats);

#endif

