#include "leitner.h"
#include <stdio.h>
#include <stddef.h>
#include <string.h>

int main(int argc, char **argv)
{
	size_t deck_sizes[256];
	char fname[256];
	int has_save_file = 0;
	int load_from_save_file = 1;
	int cards = 0;
	struct Leitner *leitner;

	if(argc > 256)
	{
		return 4;
	}
	size_t deck_size_index = 0;
	for(int i = 1; i < argc; i++)
	{
		int deck_size;

		if(strlen(argv[i]) >= 256)
		{
			fprintf(stderr, "bad argument '%s'\n", argv[i]);
			return 4;
		}
		else if(sscanf(argv[i], "-f=%s", fname) == 1)
		{
			has_save_file = 1;
			continue;
		}
		else if(sscanf(argv[i], "-c=%d", &cards) == 1)
		{
			load_from_save_file = 0;
			continue;
		}
		else if(sscanf(argv[i], "%d", &deck_size) != 1)
		{
			fprintf(stderr, "bad deck size '%s'\n", argv[i]);
			return 4;
		}
		load_from_save_file = 0;
		deck_sizes[deck_size_index++] = deck_size;
	}

	if(load_from_save_file)
	{
		if(!has_save_file)
		{
			fprintf(stderr, "daidai: usage: daidai -f=[save file] -c=[number of cards] [deck 1 size] [deck 2 size] ... OR daidai -f=[save file]\n");
			return 4;
		}
		FILE *fp = fopen(fname, "r");
		if(fp == NULL)
		{
			fprintf(stderr, "daidai: failed to open '%s' for reading\n", fname);
			return 4;
		}
		leitner = leitner_load(fp);
		fclose(fp);
	}
	else
	{
		if(!has_save_file)
		{
			fprintf(stderr, "daidai: usage: daidai -f=[save file] -c=[number of cards] [deck 1 size] [deck 2 size] ... OR daidai -f=[save file]\n");
			return 4;
		}
		if(cards == 0)
		{
			fprintf(stderr, "daidai: usage: daidai -f=[save file] -c=[number of cards] [deck 1 size] [deck 2 size] ... OR daidai -f=[save file]\n");
			return 4;
		}
		if(deck_size_index < 1)
		{
			fprintf(stderr, "daidai: usage: daidai -f=[save file] -c=[number of cards] [deck 1 size] [deck 2 size] ... OR daidai -f=[save file]\n");
			return 4;
		}
		leitner = leitner_init(deck_size_index, deck_sizes, cards);
	}
	int c;
	int card = leitner_get_next_card(leitner);
	printf("%d\n", card);
	while((c = getchar()) != 'q')
	{
		if(c == 'y')
		{
			leitner_submit(leitner, 1);
		}
		else if(c == 'n')
		{
			leitner_submit(leitner, 0);
		}
		else
		{
			continue;
		}	
		card = leitner_get_next_card(leitner);
		printf("%d\n", card);
	}

	if(has_save_file)
	{
		FILE *fp = fopen(fname, "w");
		leitner_store(leitner, fp);
		fclose(fp);
	}
	return 0;
}
