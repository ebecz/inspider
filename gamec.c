#include <locale.h>
#include <ncurses.h>
#include <stddef.h>
#include "gamec.h"
#include <stdio.h>
#include <wchar.h>
#include <string.h>

#define HEARTS     1
#define CUBS     2
#define DIAMONDS  3
#define SPADES    4

static const wchar_t *BLACK_SPADE = L"\u2660";
//static const wchar_t *WHITE_HEART = L"\u2661";
//static const wchar_t *WHITE_DIAMONDS = L"\u2662";
static const wchar_t *BLACK_CUBS = L"\u2663";
//static const wchar_t *WHITE_SPADE = L"\u2664";
static const wchar_t *BLACK_HEART = L"\u2665";
static const wchar_t *BLACK_DIAMONDS = L"\u2666";
//static const wchar_t *WHITE_CUBS = L"\u2667";


static void print_card(const struct card *card, unsigned int x, unsigned int y)
{
	static const char *names[MAX_CARD_VALUE] = {
		"ACE  ",
		"2    ",
		"3    ",
		"4    ",
		"5    ",
		"6    ",
		"7    ",
		"8    ",
		"9    ",
		"10   ",
		"Valet",
		"Queen",
		"King ",
	};
	static int suits[MAX_SUITS] = {
		HEARTS,
		CUBS,
		DIAMONDS,
		SPADES,
	};
	static const wchar_t *wsuits[MAX_SUITS];

        wsuits[0] = BLACK_SPADE;
        wsuits[1] = BLACK_HEART;
        wsuits[2] = BLACK_DIAMONDS;
        wsuits[3] = BLACK_CUBS;

	attron(COLOR_PAIR(suits[card->suit]));
	mvprintw(y, x, "[ %ls %6s ]", wsuits[card->suit], names[card->value]);
	attroff(COLOR_PAIR(suits[card->suit]));
}

static void print_cards(const struct card * const cards[], int num_cards, int coloffset)
{
	int i;
	for (i = 0; i < num_cards; i++) {
		print_card(cards[i], coloffset, i);
	}	
}

static void nstack_print(const struct stack *stack, int coloffset)
{
	print_cards(stack->pile, stack->count, coloffset);
}

void nctableau_init(struct nctableau *nctableau)
{
	tableau_init(&nctableau->tableau);
	tableau_start(&nctableau->tableau);


	setlocale(LC_ALL, "");
	initscr();
	cbreak();

	start_color();
	init_pair(HEARTS, COLOR_RED, COLOR_WHITE);
	init_pair(DIAMONDS, COLOR_RED, COLOR_WHITE);
	init_pair(CUBS, COLOR_BLACK, COLOR_WHITE);
	init_pair(SPADES, COLOR_BLACK, COLOR_WHITE);

	swprintf(nctableau->msg, GAME_MSG_SIZE, L"Welcome to the game.");
}

void nctableau_finish(const struct nctableau *nctableau)
{
	endwin();
}

static void nctableau_print(const struct nctableau *nctableau)
{
	int i;
	int row,col;
	const struct tableau *tableau = &nctableau->tableau;

	getmaxyx(stdscr, row, col);

	clear();

	for (i = 0; i < NUM_STACKS; i++) {
		nstack_print(&tableau->stacks[i], i * col / NUM_STACKS);
	}

	mvprintw(row - 1, 0, "%ls", nctableau->msg);

	refresh();
}

void nctableau_run(struct nctableau *nctableau)
{
	int cmd;
	int state = 0;
	int i;
	int stash[10] = {0, };
	for(;;){
		nctableau_print(nctableau);
		cmd = getch();
		for (i = 0; i < 10 - 1; i++){
			stash[i + 1] = stash[i];
		}
		stash[0] = cmd;
		switch(state) {
			case 1:
				swprintf(nctableau->msg, GAME_MSG_SIZE, L"Move to where? [0-9]");
				state = 2;
				break;
			case 2:
				swprintf(nctableau->msg, GAME_MSG_SIZE, L"Moved %d to %d", stash[1] - '0', stash[0] - '0');
				tableau_move(&nctableau->tableau, stash[1] - '0', stash[0] - '0');
				state = 0;
				break;
			default:
				switch(cmd) {
					case 'd':
						tableau_draw(&nctableau->tableau);
						break;
					case 'm':
						swprintf(nctableau->msg, GAME_MSG_SIZE, L"Which collumn to Move? [0-9]");
						state = 1;
						break;
					case 'q':
						return;
					default:
						swprintf(nctableau->msg, GAME_MSG_SIZE, L"Unknow command: %c", (char)cmd);
						break;
				}
				break;
		}
	}
}
