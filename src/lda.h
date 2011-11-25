#ifndef LDA_H
#define LDA_H

#include "corpus.h"

typedef struct _lda_suffstats_st {
  int *nz; // number of times the topic appears in the corpus
  int **ndz; // number of times that topic z appears in document d
  int **nzw; // number of times that topic z appears with word w

  int **topic; // topic assigments for word w in document d
  
  int num_topics;
  int num_docs;
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
void lda_estimate(lda_model_t *model, corpus_t *corpus, int max_iter,
                  int inverval, double convergence);

#endif

