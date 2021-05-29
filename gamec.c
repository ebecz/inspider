#include "gamec.h"
#include <stdio.h>

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"


static void print_card(const struct card *card)
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
	static const char *suits[MAX_SUITS] = {
		KRED,
		KNRM,
		KYEL,
		KMAG,
	};
	printf("%s%s\n", suits[card->suit], names[card->value]);
#endif
}

static void print_cards(const struct card * const cards[], int num_cards)
{
	int i;
	for (i = 0; i < num_cards; i++) {
		print_card(cards[i]);
	}	
	printf(KNRM);
}

void deck_print(const struct deck *deck)
{
	print_cards(deck->cards, NUM_CARDS_ON_A_DECK);
}

void stock_print(const struct stock *stock)
{
	print_cards(stock->pile, stock->count);
}

void stack_print(const struct stack *stack)
{
	print_cards(stack->pile, stack->count);
}

void tableau_print(const struct tableau *tableau)
{
	int i;
	for (i = 0; i < NUM_STACKS; i++) {
		printf("#####################\n");
		printf("STACK-%d\n", i);
		stack_print(&tableau->stacks[i]);
		printf("#####################\n");
	}
	printf("#####################\n");
	printf("STOCK\n\n");
	stock_print(&tableau->stock);
	printf("#####################\n");
}


