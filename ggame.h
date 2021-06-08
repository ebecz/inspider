#ifndef __GGAME_H__
#define __GGAME_H__

#include "game.h"

#define MAX_GRAPH_NODES 100000
#define MAX_MOVES 16

struct gnode {
	struct tableau tableau;
	int _entropy;
	struct {
		int entropy;
		struct gnode *node;
		struct move move;
	} moves [MAX_MOVES];
	int count;
	int it;
};

struct gtableau {
	struct gnode list[MAX_GRAPH_NODES];
	struct gnode *olist[MAX_GRAPH_NODES];
	int count;
	unsigned int depth;
};

const struct gnode *gtableau_next(struct gtableau *gtableau, const struct tableau *base);

void gtableau_init(struct gtableau *gtableau);

#endif
