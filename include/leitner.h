#ifndef LEITNER_H
#define LEITNER_H

#include <stddef.h>
#include <stdio.h>

struct Leitner;

struct Leitner *leitner_init(size_t n_decks, size_t *deck_frequencies, size_t n_cards);
int leitner_get_next_card(struct Leitner *leitner);
void leitner_submit(struct Leitner *leitner, int correct);

void leitner_store(struct Leitner *leitner, FILE *out);
struct Leitner *leitner_load(FILE *in);

#endif
