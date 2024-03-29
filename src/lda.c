#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "lda.h"
#include "utils.h"
#include "distributions.h"
#include "dirichlet.h"

#define DEFAULT_TOP_N 5


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
  stats->nwz = malloc(sizeof(int *) * m->num_terms);
  stats->topic = malloc(sizeof(int *) * c->num_docs);

  if (!stats->ndz || !stats->nwz || !stats->topic)
    die("Error allocating memory for sufficent statistics");

  for (i=0;i<c->num_docs;i++)
  {
    stats->ndz[i] = calloc(m->num_topics, sizeof(int));
    stats->topic[i] = calloc(c->docs[i].total, sizeof(int));

    if (!stats->ndz[i] || !stats->topic[i])
      die("Error allocating memory for stats in step 2!");
  }

  for (i=0;i<m->num_terms;i++)
  {
    stats->nwz[i] = calloc(m->num_topics, sizeof(int)); 
    if (!stats->nwz[i])
      die("Die error allocating memory for stats in step 3!");
  }

  return stats;
}

static void lda_destroy_suffstats(lda_suffstats_t *stats, lda_model_t *m)
{
  int i;

  for (i=0;i<stats->num_docs;i++)
  {
    free(stats->ndz[i]);
    free(stats->topic[i]);
  }
  free(stats->ndz);
  free(stats->topic);

  for (i=0;i<m->num_terms;i++)
    free(stats->nwz[i]); 
  free(stats->nwz);

  free(stats->nz);
  free(stats);
}

static void lda_compute_log_w(lda_model_t *model, lda_suffstats_t *stats) 
{
  int i, j;

  for (i=0;i<model->num_topics;i++) 
  {
    for (j=0;j<model->num_terms;j++) 
    {
      model->log_prob_w[i][j] = log((stats->nwz[j][i]+model->beta) *
                  (1.0/(stats->nz[i] + model->betaSum))); 
    }
  }
}

/*
 * Auxiliary functions for this module
 */

static double lda_perplexity(lda_model_t *model, lda_suffstats_t *stats,
                             corpus_t *c)
{
  int i, j, k;
  int word_index, word, tw=0;
  double tmp, px = 0;

  for (i=0;i<c->num_docs;i++) 
  {
    tw += c->docs[i].total;
    word_index = 0;
    for (j=0;j<c->docs[i].length;j++)
    {
      word = c->docs[i].words[j].id;
      tmp = 0;
      for (k=0;k<model->num_topics;k++) 
      {
        tmp += ((stats->nwz[word][k]+model->beta)/
                (stats->nz[k]+model->betaSum)) *
              (stats->ndz[i][k]+model->alpha[k]/
               (c->docs[i].total+model->alphaSum));
      }
      
      px += log(tmp) * c->docs[i].words[j].count;
      word_index++;
    }
  }
  
  px = exp(-(1./tw) * px);

  return px;
}

static double lda_loglikelihood(lda_model_t *model, lda_suffstats_t *stats,
                                corpus_t *c)
{
  int i, j;
  double lik = 0;
  double topic_gammas[model->num_topics];
  int nonZeroTypeTopics = 0;

  for (i=0;i<model->num_topics;i++)
    topic_gammas[i] = log_gamma(model->alpha[i]);

  // direchlet of the documents 
  for (i=0;i<c->num_docs;i++)
  {
    for (j=0;j<model->num_topics;j++) 
    {
      // if it zero then the following equation will be zero
      if (stats->ndz[i][j]==0)
        continue;

      lik += log_gamma(model->alpha[j] + stats->ndz[i][j]) -
             topic_gammas[j];
      
    }
    
    lik -= log_gamma(model->alphaSum + c->docs[i].total);
  }
  lik += stats->num_docs * log_gamma(model->alphaSum);

  // direchlet of the topics 
  for (i=0;i<model->num_topics;i++)
  {
    for (j=0;j<model->num_terms;j++) 
    {
      if (stats->nwz[j][i]==0)
        continue;

      nonZeroTypeTopics++;
      lik += log_gamma(model->beta + stats->nwz[j][i]);
    }

    lik -= log_gamma(model->beta*model->num_topics + stats->nz[i]);
  }

  lik += log_gamma(model->beta * model->num_topics) - 
          log_gamma(model->beta) * nonZeroTypeTopics;

  return lik;
}

static void lda_sample_doc(lda_model_t *model, lda_suffstats_t *stats, 
                           document_t *d, int j) 
{
  int l, p, k, z;
  int word, word_index = 0;
  double sum;
  double local_z[model->num_topics];

  for (l=0;l<d->length;l++)
  {
    // we need to do this for every repetition 
    // of the word
    word = d->words[l].id;
    for (p=0;p<d->words[l].count;p++) 
    {

      z = stats->topic[j][word_index];
      // remove this topic assigment from the statistics and model 
      // the new probability
      stats->nz[z]--;
      stats->ndz[j][z]--;
      stats->nwz[word][z]--;
      
      // now model the conditional probability of z=k 
      sum = 0;
      for (k=0;k<model->num_topics;k++) 
      {
        /* this is the inner loop where all the heavy computation
         * is done. The division is made this way because it is faster
         * than doing it directly 
         */
        local_z[k] = (model->alpha[k] + stats->ndz[j][k]) *
                     ((model->beta + stats->nwz[word][k]) * 
                      (1./(stats->nz[k] + model->betaSum)));
        
        sum += local_z[k];
      }

      /* sample a new topic for this word from the new distribution
       * and update the new stats with this new topic
       */
      
      // sample new topic
      z = random_multinomial(local_z, sum);

      stats->topic[j][word_index] = z;
      
      stats->nz[z]++;
      stats->ndz[j][z]++;
      stats->nwz[word][z]++;
      
      // increment word index (needed because on how we organize
      // memory
      word_index++;
    }
  }
}

static void lda_gibbs_initialize(lda_model_t *model, corpus_t *c,
                                 lda_suffstats_t *stats) 
{
  int i, j, k, z;
  int word, word_index;

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
        stats->nwz[word][z]++;
        
        word_index++;
      }
    }
  }
}

static void lda_gibbs_sampling(lda_model_t *model, lda_suffstats_t *stats,
                               corpus_t *c, int max_iter, int interval,
                               double cthreshold) 
{
  int i, j;
  double oldLogLikelihood, newLogLikelihood, perplexity;
  double converged;


    /* Now we start by gibbs sampling until all iterations are 
   * computed
   */
  oldLogLikelihood = 0;
  for (i=0;i<max_iter;i++)
  {

    if (interval>=0 && i!=0 && i % interval==0)
    {
      newLogLikelihood = lda_loglikelihood(model, stats, c);
      perplexity = lda_perplexity(model, stats, c);

      printf("Iteration %5d: ", i);
      printf("Log Likelihood = %10.2lf | Perplexity = %8.2lf\n",
              newLogLikelihood, perplexity);
      
      converged = fabs((oldLogLikelihood - newLogLikelihood) / oldLogLikelihood);
      if (i>interval && converged<cthreshold)
      {
        printf("Stoping after convergence met...\n");
        break;
      }
      
      oldLogLikelihood = newLogLikelihood;
    }

    for (j=0;j<c->num_docs;j++) 
    {
      lda_sample_doc(model, stats, &(c->docs[j]), j);
    }
  }
}

/* 
 * Public functions
 */

lda_model_t* lda_create(int ntopics, int nterms, double alpha, double beta) 
{
  int i;

  lda_model_t *m = malloc(sizeof(lda_model_t));
  if (m==NULL)
    die("Error allocating memory creating LDA model!");

  m->num_topics = ntopics;
  m->alphaSum = alpha * ntopics;
  m->beta = beta;
  m->betaSum = beta * nterms;
  m->num_terms = nterms;

  m->alpha = malloc(sizeof(double) * ntopics);
  if (m->alpha==NULL)
    die("Error allocating memory creating LDA model alpha parameters!");
  
  for (i=0;i<ntopics;i++)
    m->alpha[i] = alpha;

  m->log_prob_w = malloc(sizeof(double *) * m->num_topics);
  if (m->log_prob_w==NULL)
    die("Error allocating memory creating LDA model probs!");

  for (i=0;i<m->num_topics;i++)
  {
    m->log_prob_w[i] = malloc(sizeof(double) * m->num_terms);
    if (m->log_prob_w[i] == NULL)
      die("Error allocating memory creating LDA model step 2!");
  }

  return m;
}

void lda_destroy(lda_model_t *model) 
{
  int i;

  for (i=0;i<model->num_topics;i++)
    free(model->log_prob_w[i]);

  free(model->log_prob_w);
  free(model);
}

/* 
 * this function should have the various methods for learning the model 
 */
void lda_estimate(lda_model_t *model, corpus_t *c, int max_iter,
                  int interval, double convergence)
{
  
  lda_suffstats_t *stats = lda_create_suffstats(model, c);
  
  /*
   * initialization
   * randonmly make topic assigments to each word in the corpus
   */

  lda_gibbs_initialize(model, c, stats);
  lda_gibbs_sampling(model, stats, c, max_iter, interval, convergence);
  
  /* compute log prob w and print the top n words 
   */
  lda_compute_log_w(model, stats);
  lda_print_top_words(model, DEFAULT_TOP_N, stdout);
  lda_print_document_topics(model, stats, c, DEFAULT_TOP_N, stdout);

  lda_destroy_suffstats(stats, model);
}

/* 
 * print the top words of the model 
 */

typedef struct _pair_st {
  int id;
  double w;
} pair_t;

/* order pairs in decrescent order */
static int cmp_pairs(const void *a, const void *b)
{
  pair_t *pa = (pair_t *)a;
  pair_t *pb = (pair_t *)b;

  if (pa->w < pb->w)
    return 1;
  else if (pa->w == pb->w)
    return 0;
  else return -1;
}

void lda_print_top_words(lda_model_t *model, int topn, FILE *out) 
{
  int i, j;
  pair_t probs[model->num_terms];

  for (i=0;i<model->num_topics;i++) {
    fprintf(out, "Topic %d:", i);
    
    for (j=0;j<model->num_terms;j++) {
      probs[j].w = model->log_prob_w[i][j];
      probs[j].id = j;
    }

    qsort(probs, model->num_terms, sizeof(pair_t), cmp_pairs);
    for (j=0;j<topn;j++)
      fprintf(out, " %d:%lf", probs[j].id, probs[j].w);

    fprintf(out, "\n");
  }
}

/* 
 * print the topic distribution for the documents acording to the 
 * learned model
 */
void lda_print_document_topics(lda_model_t *model, lda_suffstats_t *stats,
                               corpus_t *c, int topn, FILE *out) 
{
  int i, j;
  pair_t probs[model->num_topics];

  for (i=0;i<c->num_docs;i++) 
  {
    fprintf(out, "Document %d:", i);
    
    for (j=0;j<model->num_topics;j++)
    {
      probs[j].id = j;
      probs[j].w = (stats->ndz[i][j] / (double)c->docs[i].total);
    }

    qsort(probs, model->num_topics, sizeof(pair_t), cmp_pairs);
    for (j=0;j<topn;j++)
      fprintf(out, " %d:%lf", probs[j].id, probs[j].w);
    
    fprintf(out, "\n");
  }
}

/*
 * save the trained model to a file
 */
void lda_save_model(lda_model_t *model, char *filename) 
{
  FILE *f = fopen(filename, "wb");

  fwrite(&(model->num_topics), sizeof(int), 1, f);
  fwrite(&(model->num_terms), sizeof(int), 1, f);
  fwrite(&(model->beta), sizeof(double), 1, f);
  fwrite(model->alpha, sizeof(double), model->num_topics, f);
  
  fwrite(model->log_prob_w, sizeof(double), 
         model->num_topics*model->num_terms, f);

  fclose(f);
}

/* 
 * load the model from a file
 */
lda_model_t* lda_load_model(char *filename) 
{
  double beta;
  int ntopics, nterms;
  int i;
  
  ntopics = nterms = beta = 0;
  FILE *f = fopen(filename, "rb");

  assert(fread(&ntopics, sizeof(int), 1, f)==1);
  assert(fread(&nterms, sizeof(int), 1, f)==1);
  assert(fread(&beta, sizeof(double), 1, f)==1);

  // we parameter alpha is only used when the model is being
  // created for the first time.
  // So we povide any value and override the alpha and alphasums 
  // of the model in the next step
  lda_model_t *m = lda_create(ntopics, nterms, 0, beta);
  
  assert(fread(m->alpha, sizeof(double), ntopics, f)==ntopics);
  assert(fread(m->log_prob_w, sizeof(double), 
         ntopics*nterms, f)==ntopics*nterms);

  m->alphaSum = 0;
  for (i=0;i<ntopics;i++)
    m->alphaSum += m->alpha[i];

  fclose(f);

  return m;
}


