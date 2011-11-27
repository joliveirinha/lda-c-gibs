#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "lda.h"
#include "utils.h"
#include "distributions.h"
#include "dirichlet.h"


/*
 * Sufficient statistics functions
 */

static lda_suffstats_t* lda_create_suffstats(lda_model_t *m, corpus_t *c) 
{
  int i;
  

  lda_suffstats_t *stats = malloc(sizeof(lda_suffstats_t));
  if (stats==NULL)
    die("Error allocating memory creating lda sufficient statistics!");
  
  stats->num_topics = m->num_topics;
  stats->num_docs = c->num_docs;

  stats->nz = calloc(m->num_topics, sizeof(int));
  stats->ndz = malloc(sizeof(int *) * c->num_docs);
  stats->nzw = malloc(sizeof(int *) * m->num_topics);
  stats->topic = malloc(sizeof(int *) * c->num_docs);

  if (!stats->ndz || !stats->nzw || !stats->topic)
    die("Error allocating memory for sufficent statistics");

  for (i=0;i<c->num_docs;i++)
  {
    stats->ndz[i] = calloc(m->num_topics, sizeof(int));
    stats->topic[i] = calloc(c->docs[i].total, sizeof(int));

    if (!stats->ndz[i] || !stats->topic[i])
      die("Error allocating memory for stats in step 2!");
  }

  for (i=0;i<m->num_topics;i++)
  {
    stats->nzw[i] = calloc(m->num_terms, sizeof(int)); 
    if (!stats->nzw[i])
      die("Die error allocating memory for stats in step 3!");
  }

  return stats;
}

static void lda_destroy_suffstats(lda_suffstats_t *stats)
{
  int i;

  for (i=0;i<stats->num_docs;i++)
  {
    free(stats->ndz[i]);
    free(stats->topic[i]);
  }
  free(stats->ndz);
  free(stats->topic);

  for (i=0;i<stats->num_topics;i++)
    free(stats->nzw[i]); 
  free(stats->nzw);

  free(stats->nz);
  free(stats);
}


/*
 * Auxiliary functions for this module
 */

static double lda_loglikelihood(lda_model_t *model, lda_suffstats_t *stats,
                                corpus_t *c)
{
  int i, j;
  double lik = 0;
  int nonZeroTypeTopics = 0;

  // direchlet of the documents 
  for (i=0;i<c->num_docs;i++)
  {
    for (j=0;j<model->num_topics;j++) 
    {
      if (stats->ndz[i][j] == 0)
        continue;

      lik += log_gamma(model->alpha * stats->ndz[i][j]) -
             log_gamma(model->alpha);
      
    }
    
    lik -= log_gamma(model->alpha*model->num_topics + c->docs[i].total);
  }

  printf("%lf\n", lik);
  lik += stats->num_docs * log_gamma(model->alpha*model->num_topics);
  

  // direchlet of the topics 
  for (i=0;i<model->num_topics;i++)
  {
    for (j=0;j<model->num_terms;j++) 
    {
      if (stats->nzw[i][j]==0)
        continue;

      nonZeroTypeTopics++;
      lik += log_gamma(model->beta + stats->nzw[i][j]);
    }

    lik -= log_gamma(model->beta*model->num_topics + stats->nz[i]);
  }
  lik += log_gamma(model->beta * model->num_topics) - 
         log_gamma(model->beta) * nonZeroTypeTopics;
  
  return lik;
}

static void lda_gibbs_sampling(lda_model_t *model, corpus_t *c, 
                               int max_iter, int interval,
                               double convergence) 
{
  int i, j, k, l, p;
  int word, z, word_index;
  double oldLogLikelihood, newLogLikelihood;
  double sum;

  double local_z[model->num_topics];

  lda_suffstats_t *stats = lda_create_suffstats(model, c);
  
  /*
   * randonmly make topic assigments to each word in the corpus
   */
  for (i=0;i<c->num_docs;i++)
  {
    word_index = 0;
    for (j=0;j<c->docs[i].length;j++)
    {
      word = c->docs[i].words[j].id;

      for (k=0;k<c->docs[i].words[j].count;k++) {
        z = (int) (random_uniform() * model->num_topics);
        stats->topic[i][word_index] = z;
        
        stats->nz[z]++;
        stats->ndz[i][z]++;
        stats->nzw[z][word]++;
      }

      word_index++;
    }
  }


  /* Now we start by gibbs sampling until all iterations are 
   * computed
   */
  for (i=0;i<max_iter;i++)
  {
    if (interval>=0 && i % interval==0)
    {
      newLogLikelihood = lda_loglikelihood(model, stats, c);

      printf("Iteration %d ...\n", i);
      printf("Log Likelihood %.10lf\n", newLogLikelihood);

      if (i!=0 && fabs(newLogLikelihood-oldLogLikelihood)<convergence)
      {
        printf("Stoping after convergence met...");
        break;
      }
      
      oldLogLikelihood = newLogLikelihood;
    }

    for (j=0;j<c->num_docs;j++) 
    {
      
      word_index = 0;
      for (l=0;l<c->docs[j].length;l++)
      {
        // we need to do this for every repetition 
        // of the word
        word = c->docs[j].words[l].id;
        for (p=0;p<c->docs[j].words[l].count;p++) 
        {

          z = stats->topic[j][word_index];
          // remove this topic assigment from the statistics and model 
          // the new probability
          stats->nz[z]--;
          stats->ndz[j][z]--;
          stats->nzw[z][word]--;
          
          // now model the conditional probability of z=k 
          sum = 0;
          for (k=0;k<model->num_topics;k++) 
          {
            local_z[k] = (stats->ndz[j][k] + model->alpha) *
                        ((stats->nzw[k][word] + model->beta) /
                        (stats->nz[k]+model->beta*model->num_terms));
            sum += local_z[k];
          }

          /* sample a new topic for this word from the new distribution
           * and update the new stats with this new topic
           */
          
          // sample new topic
          z = random_multinomial(local_z, model->num_topics, sum);

          stats->nz[z]++;
          stats->ndz[j][z]++;
          stats->nzw[z][word]++;
          
          // increment word index (needed because on how we organize
          // memory
          word_index++;
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

  m->log_prob_w = malloc(sizeof(double *) * m->num_terms);
  if (m->log_prob_w==NULL)
    die("Error allocating memory creating LDA model probs!");

  for (i=0;i<m->num_terms;i++)
  {
    m->log_prob_w[i] = malloc(sizeof(double) * m->num_topics);
    if (m->log_prob_w[i] == NULL)
      die("Error allocating memory creating LDA model step 2!");
  }

  return m;
}

void lda_destroy(lda_model_t *model) 
{
  int i;

  for (i=0;i<model->num_terms;i++)
    free(model->log_prob_w[i]);

  free(model->log_prob_w);
  free(model);
}

void lda_estimate(lda_model_t *model, corpus_t *c, int max_iter,
                  int interval, double convergence)
{
  lda_gibbs_sampling(model, c, max_iter, interval, convergence);
}


