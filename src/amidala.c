#include <stdio.h>
#include <stdlib.h>

#include "amidala.h"
#include "corpus.h"

#define MAX_USAGE_STR 200
#define MIN_NUM_ARGS 7

char *HELP_STR[MAX_USAGE_STR] = {
  "Usage: amidala_app k alpha beta max_iter interval convergence data_file",
  "",
  NULL,
};

static void usage() 
{
  int i;

  for (i=0;HELP_STR[i]!=NULL;i++)
    puts(HELP_STR[i]);

  exit(1);
}

static void print_lda_topics(lda_model_t *model, int topn)
{

}

int main(int argc, char **argv) 
{
  
  if (argc<=MIN_NUM_ARGS)
    usage();

  int k = atoi(argv[1]);
  double alpha = atof(argv[2]);
  double beta = atof(argv[3]);
  int max_iter = atoi(argv[4]);
  int interval = atoi(argv[5]);
  double convergence = atof(argv[6]);
  char *filename = argv[7];
    
  printf("Creating LDA model from data %s\n", filename);
  printf("Number of topics %d\n", k);
  printf("Alpha parameter %lf\n", alpha);
  printf("Beta parameter %lf\n", beta);
  printf("Number of max iterations %d\n", max_iter);
  printf("Iterations interval for calculing Log Likelyhood %d\n", interval);
  printf("Convergence value %lf\n", convergence);

  corpus_t* c = corpus_read_data(filename);
  printf("Number of documents in corpus %d\n", c->num_docs);
  printf("Vocabolary size: %d\n", c->num_terms);

  lda_model_t *model = amidala_lda(k, alpha, beta, max_iter,
                                   interval, convergence, c);
  
  print_lda_topics(model, 5);

  return 0; 
}

