#include "leitner.h"
#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

struct Leitner_Deck
{
	size_t deck_frequency;
	size_t sessions_remaining;	
	struct Queue *queue;
};
struct Leitner
{
	size_t n_decks;

	size_t current_deck;
	struct Leitner_Deck *decks;
	int *cards;
};

struct Leitner *leitner_init(size_t n_decks, size_t *deck_frequencies, size_t n_cards)
{
	/* Sanity checks */
	if(n_decks == 0 || n_cards == 0)
	{
		return NULL;
	}
	
	int found = 0;
	for(size_t i = 0; i < n_decks; i++)
	{
		if(deck_frequencies[i] != 0)
		{
			found = 1;
			break;
		}
	}
	if(!found)
	{
		return NULL;
	}
	
	struct Leitner *leitner = malloc(sizeof(struct Leitner));	
	/* Data structure initialization */
	leitner->n_decks = n_decks;
	struct Leitner_Deck *decks = malloc(sizeof(struct Leitner_Deck) * n_decks);
	if(decks == NULL)
	{
		return NULL;
	}
	leitner->decks = decks;
	leitner->cards = malloc(sizeof(int) * n_cards);
	if(leitner->cards == NULL)
	{
		free(decks);
		return NULL;
	}

	for(size_t i = 0; i < n_decks; i++)
	{
		leitner->decks[i].deck_frequency = deck_frequencies[i];
		leitner->decks[i].sessions_remaining = deck_frequencies[i];
		leitner->decks[i].queue = queue_init();		
	}
	
	for(size_t i = 0; i < n_cards; i++)
	{
		leitner->cards[i] = i;
		queue_enqueue(leitner->decks[0].queue, &(leitner->cards[i]));
	}	
	leitner->current_deck = 0;

	return leitner;
}
int leitner_get_next_card(struct Leitner *leitner)
{
	while(leitner->current_deck < leitner->n_decks)
	{
		if(leitner->decks[leitner->current_deck].sessions_remaining != 0)
		{
			leitner->current_deck++;
			continue;
		}
		if(queue_is_empty(leitner->decks[leitner->current_deck].queue))
		{
			leitner->current_deck++;
			continue;
		}
		int out = *(int *)(queue_poll(leitner->decks[leitner->current_deck].queue));
		leitner->decks[leitner->current_deck].sessions_remaining = leitner->decks[leitner->current_deck].deck_frequency;
		return out;
	}
	/* Session complete, time to reset */
	for(size_t i = 0; i < leitner->n_decks; i++)
	{
		if(leitner->decks[i].sessions_remaining != 0)
		{
			leitner->decks[i].sessions_remaining--;
		}
	}
	leitner->current_deck = 0;
	return leitner_get_next_card(leitner);
}
void leitner_submit(struct Leitner *leitner, int correct)
{
	int *card = queue_dequeue(leitner->decks[leitner->current_deck].queue);
	if(correct)
	{
		if(leitner->current_deck == leitner->n_decks - 1)
		{
			/* Put the card back into the same deck */
			queue_enqueue(leitner->decks[leitner->current_deck].queue, card);
		}
		else
		{
			/* Move it 1 deck back */
			queue_enqueue(leitner->decks[leitner->current_deck + 1].queue, card);
		}
	}
	else
	{
		if(leitner->current_deck == 0)
		{
			/* Put the card back into the same deck */
			queue_enqueue(leitner->decks[leitner->current_deck].queue, card);
		}
		else
		{
			/* Move it 1 deck forward */
			queue_enqueue(leitner->decks[leitner->current_deck - 1].queue, card);
		}
	}
	leitner->decks[leitner->current_deck].sessions_remaining--;
	leitner->current_deck++;
}

void leitner_destroy(struct Leitner *leitner)
{
	for(int i = 0; i < leitner->n_decks; i++)
	{
		queue_destroy(leitner->decks[i].queue);
	}
	free(leitner->cards);
	free(leitner->decks);
	free(leitner);
}

void leitner_store(struct Leitner *leitner, FILE *out)
{
	fwrite(&(leitner->n_decks), sizeof(size_t), 1, out);

	for(size_t i = 0; i < leitner->n_decks; i++)
	{
		size_t deck_size = queue_get_size(leitner->decks[i].queue);
		fwrite(&deck_size, sizeof(size_t), 1, out);
	}
	for(size_t i = 0; i < leitner->n_decks; i++)
	{
		fwrite(&(leitner->decks[i].deck_frequency), sizeof(size_t), 1, out);
	}
	for(size_t i = 0; i < leitner->n_decks; i++)
	{
		while(!queue_is_empty(leitner->decks[i].queue))
		{
			fwrite(queue_dequeue(leitner->decks[i].queue), sizeof(int), 1, out);
		}
	}
	leitner_destroy(leitner);
}

struct Leitner *leitner_load(FILE *in)
{
	struct Leitner *out = malloc(sizeof(struct Leitner));

	fread(&(out->n_decks), sizeof(size_t), 1, in);

	size_t *deck_sizes = malloc(sizeof(size_t) * out->n_decks);
	fread(deck_sizes, sizeof(size_t), out->n_decks, in);
	size_t *deck_frequencies = malloc(sizeof(size_t) * out->n_decks);
	fread(deck_frequencies, sizeof(size_t), out->n_decks, in);

	size_t n_cards = 0;
	for(size_t i = 0; i < out->n_decks; i++)
	{
		n_cards += deck_sizes[i];
	}

	out->decks = malloc(sizeof(struct Leitner_Deck) * out->n_decks);
	for(size_t i = 0; i < out->n_decks; i++)
	{
		out->decks[i].queue = queue_init();
	}
		
	out->cards = malloc(sizeof(int) * n_cards);
	for(size_t i = 0; i < n_cards; i++)
	{
		out->cards[i] = i;
	}

	for(size_t i = 0; i < out->n_decks; i++)
	{
		out->decks[i].deck_frequency = deck_frequencies[i];
		out->decks[i].sessions_remaining = deck_frequencies[i];
		for(size_t j = 0; j < deck_sizes[i]; j++)
		{
			int card;
			fread(&card, sizeof(int), 1, in);
			queue_enqueue(out->decks[i].queue, &(out->cards[card]));
		}
	}

	out->current_deck = 0;

	free(deck_frequencies);
	free(deck_sizes);
	return out;
}


