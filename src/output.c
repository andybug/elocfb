
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "teams.h"
#include "results.h"
#include "algorithms.h"
#include "options.h"
#include "output.h"

static struct team *sorted_teams[MAX_TEAMS];
static int (*compare_func)(const struct team*, const struct team*);

static int compare_rpi(const struct team *t1, const struct team *t2)
{
	return (t1->rpi > t2->rpi);
}

static int compare_elo(const struct team *t1, const struct team *t2)
{
	return (t1->elo > t2->elo);
}

static void add_teams_to_sort_list(void)
{
	int i;
	int added = 0;

	for (i = 0, added = 0; i < num_teams; i++) {
		sorted_teams[added] = teams + i;
		added++;
	}
}

static void shift_down(int start, int end)
{
	int root = start;
	int child;
	int swap;
	struct team *temp;

	while ((root*2) + 1 <= end) {
		child = (root*2) + 1;
		swap = root;
		if (compare_func(sorted_teams[swap], sorted_teams[child]))
			swap = child;
		if (child+1 <= end && compare_func(sorted_teams[swap], sorted_teams[child+1]))
			swap = child + 1;
		if (swap != root) {
			temp = sorted_teams[root];
			sorted_teams[root] = sorted_teams[swap];
			sorted_teams[swap] = temp;
			root = swap;
		} else
			return;
	}
}

static void heapify(void)
{
	/* start is last parent node */
	int start = (num_teams-1) / 2;

	while (start >= 0) {
		shift_down(start, num_teams-1);
		start--;
	}
}

static void sort(void)
{
	int end = num_teams - 1;
	struct team *temp;

	heapify();

	while (end > 0) {
		temp = sorted_teams[end];
		sorted_teams[end] = sorted_teams[0];
		sorted_teams[0] = temp;
		end--;
		shift_down(0, end);
	}
}

void output_to_stdout(void)
{
	int i;

	add_teams_to_sort_list();

	switch (options.output_sort_algo) {
	case ALGO_RPI:
		compare_func = compare_rpi;
		break;
	case ALGO_ELO:
		compare_func = compare_elo;
		break;
	}

	sort();

	for (i = 0; i < num_teams; i++) {
		printf("%s\t%0.4f\t%04d\n",
				sorted_teams[i]->name,
				sorted_teams[i]->rpi,
				sorted_teams[i]->elo);
	}
}
