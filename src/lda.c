#include <stdlib.h>

#include "lda.h"
#include "utils.h"

/*
 * Auxiliary functions for this module
 */

static void lda_inference(lda_model_t *model) 
{

}

static void lda_gibbs_sampling(lda_model_t *model) 
{

}


/* 
 * Public functions
 */

lda_model_t* lda_create(int ntopics, double alpha, corpus_t *c) 
{
  int i;

  lda_model_t *m = malloc(sizeof(lda_model_t));
  if (m==NULL)
    die("Error allocating memory creating LDA model!");

  m->num_topics = ntopics;
  m->alpha = alpha;
  m->num_terms = c->num_terms;

  m->log_prob_w = malloc(sizeof(double *) * ntopics);
  if (m->log_prob_w==NULL)
    die("Error allocating memory creating LDA model probs!");

  for (i=0;i<ntopics;i++)
  {
    m->log_prob_w[i] = malloc(sizeof(double) * c->num_terms);
    if (m->log_prob_w[i] == NULL)
      die("Error allocating memory creating LDA model probs!");
  }

  return m;
}

void lda_destroy(lda_model_t *model) 
{
  int i;

  for (i=0;i<model->num_topics;i++)
    free(model->log_prob_w[i]);

  free(model);
}

void lda_estimate(lda_model_t *model, corpus_t *c, int iterations)
{
  
}



