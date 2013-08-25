
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "../algorithms.h"

enum visited {
	VISIT_NONE = 0,
	VISIT_PART,
	VISIT_DONE
};

static struct algorithm *algorithms[NUM_ALGOS] = {
	&algo_rpi_def,
	&algo_elo_def
};

static struct algorithm *algo_list = NULL;
static struct algorithm *algo_list_tail = NULL;

void (*algo_hooks[NUM_ALGOS])(int, struct result*);

void add_algo(struct algorithm *algo)
{
	if (!algo_list) {
		algo_list = algo;
		algo_list_tail = algo;
	} else {
		algo_list_tail->next = algo;
		algo_list_tail = algo;
	}

	algo->next = NULL;
}

void visit_node(enum visited *states, int i)
{
	int j;

	if (states[i] == VISIT_PART) {
		fprintf(stderr, "Recursive dependencies in algorithms\n");
		exit(EXIT_FAILURE);
	}

	if (states[i] == VISIT_NONE) {
		states[i] = VISIT_PART;

		for (j = 0; j < algorithms[i]->depends.num_depends; j++)
			visit_node(states, (int) algorithms[i]->depends.depends[j] - 1);

		states[i] = VISIT_DONE;
		add_algo(algorithms[i]);
	}
}

/* Serialize algorithms by use of a topological sort */
void serialize_algos(void)
{
	enum visited states[NUM_ALGOS];
	int i;
	struct algorithm *a;

	memset(states, 0, sizeof(enum visited) * NUM_ALGOS);

	for (i = 0; i < NUM_ALGOS; i++) {
		assert((int) algorithms[i]->algo == i + 1);
		if (states[i] != VISIT_DONE)
			visit_node(states, i);
	}

	/* add the hooks from the sorted algorithms to the hooks list */
	a = algo_list;
	for (i = 0; i < NUM_ALGOS; i++) {
		assert(a != NULL);
		algo_hooks[i] = a->hook;
		a = a->next;
	}
}
