#include <locale.h>
#include <ncurses.h>
#include <stddef.h>
#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include "gamec.h"
#include "ggame.h"

#define HEARTS     1
#define CUBS     2
#define DIAMONDS  3
#define SPADES    4

#define BLACK_SPADE	L"\u2660"
#define WHITE_HEART	L"\u2661"
#define WHITE_DIAMONDS	L"\u2662"
#define BLACK_CUBS	L"\u2663"
#define WHITE_SPADE	L"\u2664"
#define BLACK_HEART	L"\u2665"
#define BLACK_DIAMONDS	L"\u2666"
#define WHITE_CUBS 	L"\u2667"


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
	static const wchar_t *wsuits[MAX_SUITS] = {
		BLACK_SPADE,
		BLACK_CUBS,
		BLACK_HEART,
		BLACK_CUBS
	};

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
	deck_init(&nctableau->deck);
	tableau_init(&nctableau->tableau, &nctableau->deck);
	tableau_start(&nctableau->tableau);

	gtableau_init(&nctableau->gtableau, &nctableau->tableau);

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

struct command {
	int cmd;
	int (*function)(struct nctableau *nctableau, const int stash[]);
	const wchar_t *querry[10];
	int num_querry;
};

int _move(struct nctableau *nctableau, const int stash[])
{
	int res = tableau_move(&nctableau->tableau, stash[1] - '0', stash[0] - '0');
	if (res == 0) {
		swprintf(nctableau->msg, GAME_MSG_SIZE, L"Moved %d to %d", stash[1] - '0', stash[0] - '0');
	} else {
		swprintf(nctableau->msg, GAME_MSG_SIZE, L"Invalid Move");
	}
	return 0;
}

int _draw(struct nctableau *nctableau, const int stash[])
{
	int res = tableau_draw(&nctableau->tableau);
	if (res < 0) {
		swprintf(nctableau->msg, GAME_MSG_SIZE, L"There is no Cards on the Stock");
	} else {
		swprintf(nctableau->msg, GAME_MSG_SIZE, L"Drow %d cards", res);
	}
	return 0;
}

int _auto(struct nctableau *nctableau, const int stash[])
{	int res = 1; //tableau_auto(&nctableau->tableau);
	if (res < 0) {
		swprintf(nctableau->msg, GAME_MSG_SIZE, L"There is no Available movements");
	} else {
		swprintf(nctableau->msg, GAME_MSG_SIZE, L"Auto Moved", res);
	}
	return 0;
}

int _quit(struct nctableau *nctableau, const int stash[])
{
	return -1;
}

#define NUM_COMMANDS 4
const struct command list[NUM_COMMANDS] =
{
	{
		.cmd = 'm',
		.num_querry = 2,
		.querry = {
			L"Which collumn to Move? [0-9]",
			L"Move to where? [0-9]",
		},
		.function = _move,
	},
	{
		.cmd = 'd',
		.num_querry = 0,
		.function = _draw,
	},
	{
		.cmd = 'a',
		.num_querry = 0,
		.function = _auto,
	},

	{
		.cmd = 'q',
		.num_querry = 0,
		.function = _quit,
	},
};

int _complain(struct nctableau *nctableau, const int stash[])
{
	swprintf(nctableau->msg, GAME_MSG_SIZE, L"Unknow command: %c", (char)stash[0]);
	return 0;
}

const struct command complain_cmd = {
	.cmd = 'd',
	.num_querry = 0,
	.function = _complain,
};

const struct command *find_command(int cmd)
{
	int i;
	for (i = 0; i < NUM_COMMANDS; i++){
		if (cmd == list[i].cmd)
			return &list[i];
	}
	return &complain_cmd;
}

void push_stash(int stash[], int size, int cmd) {
	int i;
	for (i = 0; i < size - 1; i++) {
		stash[i + 1] = stash[i];
	}
	stash[0] = cmd;
}

void nctableau_run(struct nctableau *nctableau)
{
	int cmd;
	int i;
	int stash[10] = {0, };
	const struct command *command;
	for(;;){
		nctableau_print(nctableau);
		cmd = getch();
		push_stash(stash, 10, cmd);
		command = find_command(cmd);
		for (i = 0; i < command->num_querry; i++) {
			swprintf(nctableau->msg, GAME_MSG_SIZE, L"%ls", command->querry[i]);
			nctableau_print(nctableau);
			cmd = getch();
			push_stash(stash, 10, cmd);
		}
		if (command->function(nctableau, stash) < 0) {
			return;
		}
	}
}

