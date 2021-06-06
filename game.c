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
		return NULL;
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
		const struct card *card = stock_pick(&tableau->stock);
		if (card == NULL)
			break;
		stack_push(stack, card);
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
	for (i = top; i < stack->count - 1; i++) {
		if (stack->pile[i]->value - 1 != stack->pile[i + 1]->value) {
			top = i + 1;
		}
	}
	return top;
}

static int move_cards(struct tableau *tableau, int src, int starting, int dst, struct move *move)
{
	int i;
	struct stack *src_stack = &tableau->stacks[src];
	struct stack *dst_stack = &tableau->stacks[dst];

	move->src.stack = src;
	move->src.card = src_stack->pile[starting];
	move->dst.stack = dst;
	if (dst_stack->count == 0) {
		move->dst.card = NULL;
	} else {
		move->dst.card = dst_stack->pile[dst_stack->count - 1];
	}

	for (i = starting; i < src_stack->count; i++) {
		dst_stack->pile[dst_stack->count++] = src_stack->pile[i];
	}
	src_stack->count = starting;
	return 0;

}

int tableau_move(struct tableau *tableau, unsigned int src, unsigned int dst, struct move *move)
{
	int i, top;

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

	if (dst_stack->count == 0) {
		top = find_largest_group(src_stack);
		return move_cards(tableau, src, top, dst, move);
	}

	if (dst_stack->count > MAX_CARDS_ON_A_PILE) {
		return -1;
	}

	for (top = find_largest_group(src_stack); top < src_stack->count; top++) {
		for (i = top; i < src_stack->count; i++) {
			if (dst_stack->pile[dst_stack->count - 1]->value == src_stack->pile[i]->value + 1) {
				return move_cards(tableau, src, i, dst, move);
			}
		}
	}

	return -1;
}

static int stack_entropy(const struct stack *stack)
{
	int i;
	int entropy = 0;

	if (stack->count == 0)
		return 100;

	if (stack->count == 1)
		return 0;

	for (i = 0; i < stack->count - 1; i++) {
		const struct card *a = stack->pile[i];
		const struct card *b = stack->pile[i + 1];
		if (a->value == b->value + 1) {
			if (a->suit == b->suit) {
				entropy += 0;
			} else {
				entropy += 1;
			}
		} else {
			entropy += 10;
		}
	}
	return entropy;
}

int tableau_entropy(const struct tableau *tableau)
{
	int i;
	int entropy = 0;
	for (i = 0; i < NUM_STACKS; i++)
	{
		entropy += stack_entropy(&tableau->stacks[i]);
	}
	return entropy;
}

