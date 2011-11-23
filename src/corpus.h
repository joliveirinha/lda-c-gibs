#ifndef CORPUS_H
#define CORPUS_H

/* 
 * this corpus assumes a bag of words model
 */

typedef struct _word_st {
  int id; // word id
  int count; // count
} word_t;

typedef struct _document_st {
  word_t *words;
  int length; // length of the arrays
  int total; // total number of words
} document_t;

typedef struct _corpus_st {
  document_t *docs;
  int num_terms;
  int num_docs;
} corpus_t;

corpus_t* corpus_create();
void corpus_destroy(corpus_t *c);
corpus_t* corpus_read_data(char *filename);

#endif
