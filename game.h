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


struct deck {
	struct card _cards[NUM_CARDS_ON_A_DECK];
	const struct card *cards[NUM_CARDS_ON_A_DECK];
};

struct tableau {
	struct stock stock;
	struct stack stacks[NUM_STACKS];
};

void deck_init(struct deck *deck);
void deck_sufle(struct deck *deck);

void stock_init(struct stock *stock);
void stock_fill(struct stock *stock, const struct deck *deck);
void stock_shufle(struct stock *stock);

void tableau_init(struct tableau *tableau, const struct deck *deck);
void tableau_start(struct tableau *tableau);

int tableau_draw(struct tableau *tableau);
int tableau_move(struct tableau *tableau, unsigned int src, unsigned int dst);

#endif
