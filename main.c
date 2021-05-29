#include "game.h"

int main(int argc, char *argv[])
{
	struct deck deck;
	struct stock stock;

	deck_init(&deck);
	stock_init(&stock);

	stock_fill(&stock, &deck);
	stock_fill(&stock, &deck);
	stock_shufle(&stock);

	stock_print(&stock);

}
