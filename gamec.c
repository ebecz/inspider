#include <ncurses.h>
#include "gamec.h"

#define HEARTS     1
#define CUBS     2
#define DIAMONDS  3
#define SPADES    4


static void print_card(const struct card *card, unsigned int x, unsigned int y)
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
	mvprintw(y, x, "%s\n", names[card->value]);
	attroff(COLOR_PAIR(suits[card->suit]));
}

static void print_cards(const struct card * const cards[], int num_cards, int coloffset)
{
	int i;
	for (i = 0; i < num_cards; i++) {
		print_card(cards[i], coloffset, i);
	}	
}

void ndeck_print(const struct deck *deck)
{
	print_cards(deck->cards, NUM_CARDS_ON_A_DECK, 0);
}

void nstock_print(const struct stock *stock)
{
	print_cards(stock->pile, stock->count, 0);
}

void nstack_print(const struct stack *stack, int coloffset)
{
	print_cards(stack->pile, stack->count, coloffset);
}

void tableau_print(const struct tableau *tableau)
{
	int i;
	int row,col;

	initscr();

	getmaxyx(stdscr, row, col);

	if (row);

	start_color();
	init_pair(HEARTS, COLOR_RED, COLOR_BLACK);
	init_pair(DIAMONDS, COLOR_YELLOW, COLOR_BLACK);
	init_pair(CUBS, COLOR_CYAN, COLOR_WHITE);
	init_pair(SPADES, COLOR_BLACK, COLOR_WHITE);

	for (i = 0; i < NUM_STACKS; i++) {
		nstack_print(&tableau->stacks[i], i * col / NUM_STACKS);
	}

	refresh();
	getch();
	endwin();
}

