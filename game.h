#ifndef __GAME_H__
#define __GAME_H__

#define NUM_STACKS 10
#define MAX_SUITS 4
#define MAX_CARD_VALUE 13
#define NUM_CARDS_ON_A_DECK (MAX_SUITS * MAX_CARD_VALUE)

#define MAX_CARDS_ON_A_PILE (2 * NUM_CARDS_ON_A_DECK)

struct card {
	unsigned char value;
	unsigned char suit;
};

struct stack {
	const struct card *pile[MAX_CARDS_ON_A_PILE];
	int visibles;
	int count;
};

struct stock {
	const struct card *pile[MAX_CARDS_ON_A_PILE];
	int count;
};

struct tableau {
	struct stack stacks[NUM_STACKS];
	struct stock stock;
};

struct deck {
	struct card _cards[NUM_CARDS_ON_A_DECK];
	const struct card *cards[NUM_CARDS_ON_A_DECK];
};

#endif
