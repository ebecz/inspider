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

void tableau_init(struct tableau *tableau, const struct deck *deck)
{
	memset(tableau, 0, sizeof(*tableau));
	stock_init(&tableau->stock);

	stock_fill(&tableau->stock, deck);
	stock_fill(&tableau->stock, deck);
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
	const int INITIAL_DRAW = 54;
	for (i = 0; i < INITIAL_DRAW; i++) {
		struct stack *stack = &tableau->stacks[i % NUM_STACKS];
		stack_push(stack, stock_pick(&tableau->stock));
	}
}

int tableau_draw(struct tableau *tableau)
{
	int i;
	if (tableau->stock.count == 0) {
		return -1;
	}
	for (i = 0; i < NUM_STACKS; i++) {
		struct stack *stack = &tableau->stacks[i];
		const struct card *card = stock_pick(&tableau->stock);
		if (card == NULL)
			break;
		stack_push(stack, card);
	}
	return i;
}

int find_largest_group(struct stack *stack)
{
	int i;
	int top = 0;
	for (i = 0; i < stack->count; i++) {
		if (stack->pile[i]->suit != stack->pile[top]->suit) {
			top = i;
		}
	}
	return top;
}

int tableau_move(struct tableau *tableau, unsigned int src, unsigned int dst)
{
	int i, j, top;
	if (src > NUM_STACKS) {
		return -1;
	}
	if (src > NUM_STACKS || dst > NUM_STACKS) {
		return -1;
	}

	if (src == dst) {
		return -1;
	}

	struct stack *src_stack = &tableau->stacks[src];
	struct stack *dst_stack = &tableau->stacks[dst];

	if (src_stack->count == 0) {
		return -1;
	}

	if (dst_stack->count > MAX_CARDS_ON_A_PILE) {
		return -1;
	}

	for (top = find_largest_group(src_stack); top < src_stack->count; top++) {
		for (i = top; i < src_stack->count; i++) {
			if (dst_stack->pile[dst_stack->count - 1]->value == src_stack->pile[i]->value + 1) {
				for (j = i; j < src_stack->count; j++) {
					dst_stack->pile[dst_stack->count++] = src_stack->pile[j];
				}
				src_stack->count = i;
				return 0;
			}
		}
	}

	return -1;
}

