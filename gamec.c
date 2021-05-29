#include <ncurses.h>
#include "gamec.h"

#define HEARTS     1
#define CUBS     2
#define DIAMONDS  3
#define SPADES    4


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
	static int suits[MAX_SUITS] = {
		HEARTS,
		CUBS,
		DIAMONDS,
		SPADES,
	};
	attron(COLOR_PAIR(suits[card->suit]));
	printw("%s\n", names[card->value]);
	attroff(COLOR_PAIR(suits[card->suit]));
}

static void print_cards(const struct card * const cards[], int num_cards)
{
	int i;
	for (i = 0; i < num_cards; i++) {
		print_card(cards[i]);
	}	
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
	initscr();

	start_color();
	init_pair(HEARTS, COLOR_RED, COLOR_MAGENTA);
	init_pair(DIAMONDS, COLOR_YELLOW, COLOR_GREEN);
	init_pair(CUBS, COLOR_CYAN, COLOR_BLUE);
	init_pair(SPADES, COLOR_BLACK, COLOR_WHITE);

	for (i = 0; i < NUM_STACKS; i++) {
		printw("#####################\n");
		printw("STACK-%d\n", i);
		stack_print(&tableau->stacks[i]);
		printw("#####################\n");
	}
	printw("#####################\n");
	printw("STOCK\n\n");
	stock_print(&tableau->stock);
	printw("#####################\n");

	refresh();
	getch();
	endwin();
}
