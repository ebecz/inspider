#ifndef __GGAME_H__
#define __GGAME_H__

#include "game.h"

#define MAX_GRAPH_NODES 1000
#define MAX_MOVES 16

struct gnode {
	int entropy;
	struct tableau tableau;
	struct {
		const struct gnode *node;
		struct move move;
	} moves [MAX_MOVES];
	int count;
};

struct gtableau {
	struct gnode list[MAX_GRAPH_NODES];
	int count;
};

void gtableau_init(struct gtableau *gtableau, const struct tableau *base);

const struct gnode *gtableau_next(struct gtableau *gtableau, const struct tableau *base);

#endif
