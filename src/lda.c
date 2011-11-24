#include <stdlib.h>

#include "lda.h"
#include "utils.h"

/*
 * Auxiliary functions for this module
 */

static void lda_inference(lda_model_t *model) 
{

}

static void lda_gibbs_sampling(lda_model_t *model, corpus_t *c, int iterations) 
{
  int i, j, k, l, p;
  int word, topic;
  
  lda_suffstats_t *stats = lda_create_suffstats(model, c);
  
  /*
   * randonmly make topic assigments to each word in the corpus
   */


  /* Now we start by gibbs sampling until all iterations are 
   * computed
   */
  for (i=0;i<iterations;i++)
  {
    if (i % 10 == 0)
      printf("Iteration %d ...\n", i);

    for (j=0;j<c->num_docs;j++) 
    {
      
      for (l=0;l<c->docs[j].length;l++)
      {
        // we need to do this for every repetition 
        // of the word
        for (p=0;p<c->docs[j].words[l].count;p++) 
        {

          word = c->docs[j].words[l].id;
          //topic = stats->df  get the topic assigment for specific word
          
          // remove this topic assigment from the statistics and model 
          // the new probability

          for (k=0;k<model->num_topics;k++) 
          {
            /* calc the probablilty of the topic being K for this word
             */
            //stats->
            
          }

          /* sample a new topic for this word from the new distribution
           * and update the new stats with this new topic
           */

          //topic = 0;
      
        }
      } 
    }
  }

  lda_destroy_suffstats(stats);
}

/* 
 * Public functions
 */

lda_model_t* lda_create(int ntopics, double alpha, double beta, corpus_t *c) 
{
  int i;

  lda_model_t *m = malloc(sizeof(lda_model_t));
  if (m==NULL)
    die("Error allocating memory creating LDA model!");

  m->num_topics = ntopics;
  m->alpha = alpha;
  m->beta = beta;
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
  lda_gibbs_sampling(model, c, iterations);
}


/*
 * Sufficient statistics functions
 */

lda_suffstats_t* lda_create_suffstats(lda_model_t *m, corpus_t *c) 
{
  lda_suffstats_t *stats = malloc(sizeof(lda_suffstats_t *));

  return stats;
}

void lda_destroy_suffstats(lda_suffstats_t *stats)
{
  free(stats);
}

