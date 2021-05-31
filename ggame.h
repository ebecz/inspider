#ifndef __GGAME_H__
#define __GGAME_H__

#include "game.h"

#define MAX_GRAPH_NODES 1000
#define MAX_MOVES 16

struct gnode {
	struct tableau tableau;
	const struct gnode *moves[MAX_MOVES];
	int count;
};

struct gtableau {
	struct gnode list[MAX_GRAPH_NODES];
	int count;
};

void gtableau_init(struct gtableau *gtableau, const struct tableau *base);

int gtableau_auto(struct tableau *tableau);

#endif
