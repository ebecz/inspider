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

static int card_string(const struct card *card, wchar_t *buffer, size_t len)
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
	static const wchar_t *wsuits[MAX_SUITS] = {
		BLACK_SPADE,
		BLACK_CUBS,
		BLACK_HEART,
		BLACK_CUBS
	};
	return swprintf(buffer, len, L"[ %ls %6s ]", wsuits[card->suit], names[card->value]);
}

static void print_card(const struct card *card, unsigned int x, unsigned int y)
{
	wchar_t buffer[16];
	static int suits[MAX_SUITS] = {
		HEARTS,
		CUBS,
		DIAMONDS,
		SPADES,
	};

	attron(COLOR_PAIR(suits[card->suit]));
	card_string(card, buffer, 16);
	mvprintw(y, x, "%ls", buffer);
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

	if (nctableau->options != NULL) {
		const struct gnode *node = nctableau->options;
		for (i = 0; i < node->count; i++)
		{
			const struct move *move = &node->moves[i].move;
			wchar_t src_name[16];
			wchar_t dst_name[16];
			card_string(move->src.card, src_name, 16);
			card_string(move->dst.card, dst_name, 16);
			mvprintw(row - 1 + i - node->count, 0,
					"%d) Move %ls at %d to %ls at %d", i,
					src_name, move->src.stack, dst_name, move->dst.stack);

		}
	}

	mvprintw(row - 1, 0, "%ls", nctableau->msg);

	refresh();
}

struct command {
	int cmd;
	int (*function)(struct nctableau *nctableau, const int stash[], int step);
};

int _move(struct nctableau *nctableau, const int stash[], int step)
{
	struct move move;
	int res;

	switch(step) {
		case 0:
			swprintf(nctableau->msg, GAME_MSG_SIZE, L"Which collumn to Move? [0-9]");
			return 1;
		case 1:
			swprintf(nctableau->msg, GAME_MSG_SIZE, L"Move to where? [0-9]");
			return 2;
		case 2:
			res = tableau_move(&nctableau->tableau, stash[1] - '0', stash[0] - '0', &move);
			if (res == 0) {
				wchar_t src_name[16];
				wchar_t dst_name[16];
				card_string(move.src.card, src_name, 16);
				card_string(move.dst.card, dst_name, 16);
				swprintf(nctableau->msg, GAME_MSG_SIZE, L"Moved %ls at %d to %ls at %d",
						src_name, move.src.stack, dst_name, move.dst.stack);
			} else {
				swprintf(nctableau->msg, GAME_MSG_SIZE, L"Invalid Move");
			}
			return 0;
		default:
			return 0;
	}
}

int _draw(struct nctableau *nctableau, const int stash[], int step)
{
	int res;
	switch(step) {
		case 0:
			res = tableau_draw(&nctableau->tableau);
			if (res < 0) {
				swprintf(nctableau->msg, GAME_MSG_SIZE, L"There is no Cards on the Stock");
			} else {
				swprintf(nctableau->msg, GAME_MSG_SIZE, L"Drow %d cards", res);
			}
			return 0;
		default:
			return 0;
	}
}

int _help(struct nctableau *nctableau, const int stash[], int step)
{
	const struct gnode *node;
	switch(step) {
		case 0:
			node = gtableau_next(&nctableau->gtableau, &nctableau->tableau);
			if (node == NULL) {
				swprintf(nctableau->msg, GAME_MSG_SIZE, L"Unable to find free movements");
				nctableau->options = NULL;
				return 0;
			}

			if (node->count == 0) {
				swprintf(nctableau->msg, GAME_MSG_SIZE, L"There is no Available movements");
				nctableau->options = NULL;
			} else {
				swprintf(nctableau->msg, GAME_MSG_SIZE, L"Found %d moves", node->count);
				nctableau->options = node;
			}
			return 0;
		default:
			return 0;
	}
}

int _quit(struct nctableau *nctableau, const int stash[], int step)
{
	switch(step) {
		case 0:
			return -1;
		default:
			return 0;
	}
}

#define NUM_COMMANDS 4
const struct command list[NUM_COMMANDS] =
{
	{
		.cmd = 'm',
		.function = _move,
	},
	{
		.cmd = 'd',
		.function = _draw,
	},
	{
		.cmd = 'h',
		.function = _help,
	},
	{
		.cmd = 'q',
		.function = _quit,
	},
};

int _complain(struct nctableau *nctableau, const int stash[], int step)
{
	switch(step) {
		case 0:
			swprintf(nctableau->msg, GAME_MSG_SIZE, L"Unknow command: %c", (char)stash[0]);
			return 0;
		default:
			return 0;
	}
}

const struct command complain_cmd = {
	.cmd = 'd',
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
	int cmd, res;
	int stash[10] = {0, };
	const struct command *command;
	do {
		nctableau_print(nctableau);
		cmd = getch();
		push_stash(stash, 10, cmd);
		command = find_command(cmd);
		res = command->function(nctableau, stash, 0);
		while (res > 0) {
			nctableau_print(nctableau);
			cmd = getch();
			push_stash(stash, 10, cmd);
			res = command->function(nctableau, stash, res);
		};
	} while(res >= 0);
}

