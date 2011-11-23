#include <stdio.h>
#include <stdlib.h>

#include "amidala.h"
#include "corpus.h"

#define MAX_USAGE_STR 200
#define MIN_NUM_ARGS 4

char *HELP_STR[MAX_USAGE_STR] = {
  "Usage: amidala_app k alpha iterations data_file",
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
  
  if (argc<MIN_NUM_ARGS)
    usage();

  int k = atoi(argv[1]);
  double alpha = atof(argv[2]);
  int iterations = atoi(argv[3]);
    
  printf("Creating LDA model from data %s\n", argv[4]);
  printf("Number of topics %d\n", k);
  printf("Alpha parameter %lf\n", alpha);
  printf("Number of iterations %d\n", iterations);

  corpus_t* c = corpus_read_data(argv[4]);
  printf("Number of documents in corpus %d\n", c->num_docs);

  lda_model_t *model = amidala_lda(k, alpha, iterations, c);
  
  print_lda_topics(model, 5);

  return 0; 
}

