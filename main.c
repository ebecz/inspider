#include "game.h"

int main(int argc, char *argv[])
{
	struct tableau tableau;

	tableau_init(&tableau);

	tableau_print(&tableau);
}
