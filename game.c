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

static void shufle_cards(const struct card **cards, int num_cards)
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

void deck_sufle(struct deck *deck)
{
	shufle_cards(deck->cards, NUM_CARDS_ON_A_DECK);
}

void stock_shufle(struct stock *stock)
{
	shufle_cards(stock->pile, stock->count);
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

void tableau_init(struct tableau *tableau)
{
	memset(tableau, 0, sizeof(*tableau));
	deck_init(&tableau->deck);
	stock_init(&tableau->stock);

	stock_fill(&tableau->stock, &tableau->deck);
	stock_fill(&tableau->stock, &tableau->deck);
	stock_shufle(&tableau->stock);
}

static const struct card *stock_pick(struct stock *stock)
{
	if (stock->count == 0) {
		error(-1, 0, "Unable to draw from a empty stock");
	}
	return stock->pile[--stock->count];
}

static void stack_push(struct stack *stack, const struct card *card)
{
	if (stack->count > MAX_CARDS_ON_A_PILE) {
		error(-1, 0, "Unable to push more than %d cards into a stack", MAX_CARDS_ON_A_PILE);
	}
	if (card == NULL) {
		error(-1, 0, "Unable to push a non existant card into stack");
	}
	stack->pile[stack->count++] = card;
}

void tableau_start(struct tableau *tableau)
{
	int i;
	const int INITIAL_DRAW = 52;
	for (i = 0; i < INITIAL_DRAW; i++) {
		struct stack *stack = &tableau->stacks[i % NUM_STACKS];
		stack_push(stack, stock_pick(&tableau->stock));
	}
}

