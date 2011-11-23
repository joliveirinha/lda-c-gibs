#include <stdio.h>
#include <stdlib.h>

#include "corpus.h"
#include "amidala.h"

corpus_t* corpus_create()
{
  corpus_t *c = malloc(sizeof(corpus_t));
  c->docs = NULL;
  c->num_terms = 0;
  c->num_docs = 0;

  return c;
}

void corpus_destroy(corpus_t *c) 
{
  int i;
  
  for (i=0;i<c->num_docs;i++)
    free(c->docs[i].words);

  free(c->docs);
  free(c);
}

corpus_t* corpus_read_data(char *filename) 
{
  int i, ndocs, nterms;
  int wid, count;
  int length;
  FILE *infile = fopen(filename, "r");

  corpus_t *corpus = corpus_create();

  ndocs = nterms = 0;
  while ((fscanf(infile, "%10d", &length) != EOF))
  {
    corpus->docs = realloc(corpus->docs, sizeof(document_t) * (ndocs+1));
    corpus->docs[ndocs].length = length;
    corpus->docs[ndocs].total = 0;
    
    corpus->docs[ndocs].words = malloc(sizeof(word_t) * length);

    for (i=0;i<length;i++) 
    {
      fscanf(infile, "%10d:%10d", &wid, &count);
      corpus->docs[ndocs].words[i].id = wid;
      corpus->docs[ndocs].words[i].count = count;
      corpus->docs[ndocs].total += count;

      nterms = MAX(wid, nterms+1);
    }

    ndocs++;
  }

  corpus->num_docs = ndocs;
  
  fclose(infile);
  
  return corpus;
}
