#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <error.h>
#include "game.h"

void deck_init(struct deck *deck)
{
	int i, j, n = 0;
	for (i = 0; i < MAX_SUITS; i++) {
		for (j = 0; j < MAX_CARD_VALUE; j++) {
			deck->_cards[n].value = j;
			deck->_cards[n].suit = i;
			deck->cards[n] = &deck->_cards[n];
			n++;
		}
	}
}

void shufle_cards(const struct card **cards, int num_cards)
{
	int i;
	for (i = 0; i < num_cards; i++)
	{
		int swap = rand() % num_cards;
		const struct card *save = cards[swap];
		cards[swap] = cards[i];
		cards[i] = save;
	}	
}

void shufle_deck(struct deck *deck)
{
	shufle_cards(deck->cards, NUM_CARDS_ON_A_DECK);
}

void stock_shufle(struct stock *stock)
{
	shufle_cards(stock->pile, stock->count);
}

void print_card(const struct card *card)
{
	static const char *names[MAX_CARD_VALUE] = {
		"ACE",
		"2",
		"3",
		"4",
		"5",
		"6",
		"7",
		"8",
		"9",
		"10",
		"Valet",
		"Queen",
		"King",
	};
#if 0
	static const char *suits[MAX_SUITS] = {
		"Hearts",
		"Spade",
		"Clubs",
		"Diamonds",
	};
	printf("%s %s\n", names[card->value], suits[card->suit]);
#else
#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"
	static const char *suits[MAX_SUITS] = {
		KRED,
		KNRM,
		KYEL,
		KMAG,
	};
	printf("%s%s\n", suits[card->suit], names[card->value]);
#endif
}

void print_cards(const struct card * const cards[], int num_cards)
{
	int i;
	for (i = 0; i < num_cards; i++) {
		print_card(cards[i]);
	}	
}

void print_deck(const struct deck *deck)
{
	print_cards(deck->cards, NUM_CARDS_ON_A_DECK);
}

void stock_print(const struct stock *stock)
{
	print_cards(stock->pile, stock->count);
}

void stock_init(struct stock *stock)
{
	memset(stock, 0, sizeof(*stock));
}

void stock_fill(struct stock *stock, const struct deck *deck)
{
	int i;
	for (i = 0; i < NUM_CARDS_ON_A_DECK; i++)
	{
		stock->pile[stock->count++] = deck->cards[i];
		if (stock->count > MAX_CARDS_ON_A_PILE) 
		{
			error(-1, 0, "Unable to push more than %d cards into the stock", MAX_CARDS_ON_A_PILE);
		}
	}
}
