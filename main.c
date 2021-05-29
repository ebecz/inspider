#include <stdio.h>
#include <stdlib.h>

#define MAX_CARDS_ON_A_PILE 200
#define NUM_STACK 10
#define MAX_SUITS 4
#define MAX_CARD_VALUE 13
#define NUM_CARDS_ON_A_DECK (MAX_SUITS * MAX_CARD_VALUE)

struct card {
	unsigned char visible;
	unsigned char value;
	unsigned char suit;
};

struct stack {
	struct card pile[MAX_CARDS_ON_A_PILE];
	int count;
};

struct tableau {
	struct stack stacks[NUM_STACK];
};

struct deck {
	struct card _cards[NUM_CARDS_ON_A_DECK];
	struct card *cards[NUM_CARDS_ON_A_DECK];
};

void fill_deck(struct deck *deck)
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

void shufle_deck(struct deck *deck)
{
	int i;
	for (i = 0; i < NUM_CARDS_ON_A_DECK; i++)
	{
		int swap = rand() % NUM_CARDS_ON_A_DECK;
		struct card *save = deck->cards[swap];
		deck->cards[swap] = deck->cards[i];
		deck->cards[i] = save;
	}	
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

void print_deck(const struct deck *deck)
{
	int i;
	for (i = 0; i < NUM_CARDS_ON_A_DECK; i++) {
		print_card(deck->cards[i]);
	}	
}

int main(int argc, char *argv[])
{
	struct deck deck[2];
	fill_deck(&deck[0]);
	fill_deck(&deck[1]);

	print_deck(&deck[0]);
	printf("-----------");
	shufle_deck(&deck[0]);
	print_deck(&deck[0]);
}
