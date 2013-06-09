
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

static size_t find_longest_name(void)
{
	size_t len;
	size_t longest = 0;
	int i;

	for (i = 0; i < num_teams; i++) {
		len = strlen(teams[i].name);
		if (len > longest)
			longest = len;
	}

	return longest;
}

static void print_padding(size_t len, size_t longest)
{
	size_t num, i;

	/* add 4 spaces so the longest name won't run up against the values */
	num = (longest-len) + 4;

	for (i = 0; i < num; i++)
		putchar(' ');
}

void output_to_stdout(void)
{
	int i, num, max_teams;
	size_t len, longest;
	bool needs_tab;

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
	longest = find_longest_name();

	/* set the number of teams to display based on output_max_teams opt */
	max_teams = options.output_max_teams;
	num = max_teams == -1 ? num_teams : max_teams;

	for (i = 0; i < num; i++) {
		len = (size_t) printf("%s", sorted_teams[i]->name);
		print_padding(len, longest);
		needs_tab = false;

		if (options.output_rpi) {
			printf("%0.4f", sorted_teams[i]->rpi);
			needs_tab = true;
		}

		if (options.output_elo) {
			if (needs_tab)
				putchar('\t');

			printf("%04d", sorted_teams[i]->elo);
			needs_tab = true;
		}

		putchar('\n');
	}
}
