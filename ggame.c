#include "ggame.h"
#include "string.h"

static int compare_stacks(const struct stack *a, const struct stack *b)
{
	if (a->count != b->count)
		return a->count - b->count;

	if (a->visibles != b->visibles)
		return a->visibles - b->visibles;

	return memcmp(a->pile, b->pile, sizeof(struct card *) * a->count);
}

static int compare_tableau(const struct tableau *a, const struct tableau *b)
{
	int i;
	if (a->stock.count != b->stock.count)
		return a->stock.count - b->stock.count;

	for (i = 0; i < NUM_STACKS; i++) {
		if (compare_stacks(&a->stacks[i], &b->stacks[i]) != 0) {
			return -1;
		}
	}
	return 0;
}

static struct gnode *find_cached_node(struct gtableau *gtableau, const struct tableau *tableau)
{
	int i;
	for(i = 0; i < gtableau->count; i++) {
		if (compare_tableau(&gtableau->list[i].tableau, tableau) == 0) {
			return &gtableau->list[i];
		}
	}
	return NULL;
}

static void keep_node(struct gtableau *gtableau, struct gnode *next)
{
	next->entropy = tableau_entropy(&next->tableau);
	gtableau->count++;
}

static void release_node(struct gtableau *gtableau, const struct gnode * next)
{

}

static int add_link(struct gnode *src, struct gnode *dst, const struct move *move)
{
	int i;
	for (i = 0; i < src->count; i++) {
		if (src->moves[i].node == dst)
			return 0;
	}

	if (src->count >= MAX_MOVES)
		return -1;

	src->moves[src->count].node = dst;
	src->moves[src->count].move = *move;
	src->count++;
	return 0;
}

static struct gnode *fetch_free_node(struct gtableau *gtableau)
{
	if (gtableau->count >= MAX_GRAPH_NODES)
		return NULL;

	struct gnode *node = &gtableau->list[gtableau->count];
	return node;
}

static int gtableau_fill(struct gtableau *gtableau, struct gnode *node)
{
	struct move move;
	struct gnode *cached;
	struct gnode *next;
	int i, j;
	int res;

	next = fetch_free_node(gtableau);
	if (next == NULL)
		return -1;

	for (i = 0; i < NUM_STACKS; i++) {
		for (j = 0; j < NUM_STACKS; j++) {
			next->tableau = node->tableau;
			res = tableau_move(&next->tableau, i, j, &move);
			if (res == 0) {
				cached = find_cached_node(gtableau, &next->tableau);
				if (cached == NULL) {
					if (add_link(node, next, &move) < 0)
						return -1;
					keep_node(gtableau, next);
					next = fetch_free_node(gtableau);
					if (next == NULL)
						return -1;
				} else {
					if (add_link(node, cached, &move) < 0)
						return -1;
				}
			}
		}
	}
	release_node(gtableau, next);
	return 0;
}

const struct gnode *gtableau_next(struct gtableau *gtableau, const struct tableau *base)
{
	struct gnode *node = find_cached_node(gtableau, base);
	if (node == NULL) {

		node = fetch_free_node(gtableau);
		if (node == NULL)
			return NULL;

		node->tableau = *base;
		keep_node(gtableau, node);
	}
	gtableau_fill(gtableau, node);
	return node;
}

void gtableau_init(struct gtableau *gtableau, const struct tableau *base)
{
	memset(gtableau, 0, sizeof(*gtableau));
}

