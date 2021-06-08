#include "ggame.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

static int compare_stacks_up(const struct stack *a, const struct stack *b)
{
	if (a->count != b->count)
		return a->count - b->count;

	if (a->visibles != b->visibles)
		return a->visibles - b->visibles;

	return 0;
}

static int compare_stacks_down(const struct stack *a, const struct stack *b)
{
	int i;
	for (i = a->count; i > 0; i--) {
		int j = i - 1;
		if (a->pile[j] != b->pile[j]) 
			return a->pile[j] != b->pile[j];
	}
	return 0;
}


static int compare_tableau(const struct tableau *a, const struct tableau *b)
{
	int i;
	int res;
	if (a->stock.count != b->stock.count)
		return a->stock.count - b->stock.count;

	for (i = 0; i < NUM_STACKS; i++) {
		res = compare_stacks_up(&a->stacks[i], &b->stacks[i]);
		if (res != 0) {
			return res;
		}
	}

	for (i = 0; i < NUM_STACKS; i++) {
		res = compare_stacks_down(&a->stacks[i], &b->stacks[i]);
		if (res != 0) {
			return res;
		}
	}

	return 0;
}

static struct gnode *find_cached_node_ab(struct gtableau *gtableau, const struct tableau *tableau, int a, int b)
{
	int res;
	int c;

	if (a > b)
		return NULL;

	c = (a + b) / 2;

	res = compare_tableau(&gtableau->olist[c]->tableau, tableau);
	if (res == 0)
		return gtableau->olist[c];

        if (res < 0) { 
		return find_cached_node_ab(gtableau, tableau, a, c - 1);
	}

	if (res > 0) {
		return find_cached_node_ab(gtableau, tableau, c + 1, b);
	}

	return NULL;
}

static struct gnode *find_cached_node(struct gtableau *gtableau, const struct tableau *tableau)
{
	return find_cached_node_ab(gtableau, tableau, 0, gtableau->count - 1);
}

int cmpfunc (const void * a, const void * b) {
	const struct gnode *na = a;
	const struct gnode *nb = b;
	return compare_tableau(&na->tableau, &nb->tableau);
}

static void keep_node(struct gtableau *gtableau, struct gnode *next)
{
	gtableau->olist[gtableau->count] = next;
	gtableau->count++;
	qsort(gtableau->olist, gtableau->count, sizeof(const struct gnode *), cmpfunc);
	next->_entropy = tableau_entropy(&next->tableau);
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
	node->count = 0;
	node->it = 0;
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

int node_add_draw(struct gtableau *gtableau, struct gnode *node)
{
	int res;
	struct gnode *next, *cached;
	static struct move draw_move = { .draw = 1 };

	next = fetch_free_node(gtableau);
	if (next == NULL)
		return -1;

	next->tableau = node->tableau;
	res = tableau_draw(&next->tableau);
	if (res == 0) {
		release_node(gtableau, next);
		return 0;
	}

	cached = find_cached_node(gtableau, &next->tableau);
	if (cached == NULL) {
		keep_node(gtableau, next);
		if (add_link(node, next, &draw_move) < 0)
			return -1;
		return 0;
	} else {
		release_node(gtableau, next);
		if (add_link(node, cached, &draw_move) < 0)
			return -1;
	}
	return 0;
}

static void gtableau_map(struct gtableau *gtableau, struct gnode *node, unsigned int depth)
{
	int i;

	if (node->it)
		return;

	if (depth == 0) 
		return;

	node->it = 1;
	gtableau_fill(gtableau, node);
	//node_add_draw(gtableau, node);
	for (i = 0; i < node->count; i++) {
		gtableau_map(gtableau, node->moves[i].node, depth - 1);
	}
}

static struct gnode *ensure_node(struct gtableau *gtableau, const struct tableau *base)
{
	struct gnode *node = find_cached_node(gtableau, base);
	if (node == NULL) {

		node = fetch_free_node(gtableau);
		if (node == NULL)
			return NULL;

		node->tableau = *base;
		keep_node(gtableau, node);
	}
	return node;
}

static int update_entropy_tree(struct gnode *node, int it)
{
	int i;

	if (node->it == it)
		return node->_entropy + 2;

	if (node->count == 0)
		return node->_entropy + 1;

	int best_entropy = node->_entropy;
	int entropy;
	node->it = it;
	for (i = 0; i < node->count; i++) {
		entropy = update_entropy_tree(node->moves[i].node, it);
		node->moves[i].entropy = entropy;
		if (entropy < best_entropy) {
			entropy = best_entropy;
		}
	}
	return best_entropy;
}

const struct gnode *gtableau_next(struct gtableau *gtableau, const struct tableau *base)
{
	gtableau->count = 0;
	struct gnode *node = ensure_node(gtableau, base);
	if (node == NULL)
		return NULL;

	gtableau_map(gtableau, node, gtableau->depth);
	update_entropy_tree(node, 2);
	return node;
}

void gtableau_init(struct gtableau *gtableau)
{
	gtableau->depth = 4;
}
