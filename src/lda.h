#ifndef LDA_H
#define LDA_H

#include <stdio.h>

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
  double *alpha;
  double alphaSum;
  double beta;
  double betaSum;

  double **log_prob_w;
  int num_topics;
  int num_terms;
} lda_model_t;


lda_model_t* lda_create(int ntopics, int nterms, double alpha, double beta);
void lda_destroy(lda_model_t *model);
void lda_estimate(lda_model_t *model, corpus_t *corpus, int max_iter,
                  int inverval, double convergence);
void lda_print_top_words(lda_model_t *model, int topn, FILE *out);
void lda_print_document_topics(lda_model_t *model, corpus_t *c, FILE *out);
void lda_save_model(lda_model_t *m, char *filename);
lda_model_t* lda_load_model(char *filename);

#endif

