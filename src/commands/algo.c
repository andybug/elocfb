
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../commands.h"
#include "../teams.h"
#include "../results.h"
#include "../parse.h"
#include "../algorithms.h"

static struct team *sorted_teams[MAX_TEAMS];
static int (*compare_func)(const struct team*, const struct team*);


static int compare_winper(const struct team *t1, const struct team *t2)
{
	return (t1->winper > t2->winper);
}

static int compare_rpi(const struct team *t1, const struct team *t2)
{
	return (t1->rpi > t2->rpi);
}

static int compare_elo(const struct team *t1, const struct team *t2)
{
	return (t1->elo > t2->elo);
}


static void display_winper(void)
{
	int i;

	for (i = 0; i < num_teams; i++) {
		printf("%0.4f\t%s\n",
				sorted_teams[i]->winper,
				sorted_teams[i]->name);
	}
}

static void display_rpi(void)
{
	int i;

	for (i = 0; i < num_teams; i++) {
		printf("%0.4f\t%s\n",
				sorted_teams[i]->rpi,
				sorted_teams[i]->name);
	}
}

static void display_elo(void)
{
	int i;

	for (i = 0; i < num_teams; i++) {
		printf("%04d\t%s\n",
				sorted_teams[i]->elo,
				sorted_teams[i]->name);
	}
}


static void add_teams_to_sort_list(void)
{
	int i;
	int added;

	for (i = 0, added = 0; i < MAX_TEAMS; i++) {
		if (teams[i].key == 0)
			continue;

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

static void heapify()
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

int cmd_algo(int argc, char **argv)
{
	void (*display_func)(void) = NULL;

	if (argc != 4)
		return EXIT_FAILURE;

	init_teams();
	parse_ncaa(argv[3]);
	sort_results();

	if (strcmp("winper", argv[2]) == 0) {
		algo_winper();
		compare_func = compare_winper;
		display_func = display_winper;
	} else if (strcmp("rpi", argv[2]) == 0) {
		algo_winper();
		algo_rpi();
		compare_func = compare_rpi;
		display_func = display_rpi;
	} else if (strcmp("elo", argv[2]) == 0) {
		algo_winper();
		algo_rpi();
		algo_elo();
		compare_func = compare_elo;
		display_func = display_elo;
	} else {
		fprintf(stderr, "Invalid algorithm '%s'\n", argv[2]);
		return EXIT_FAILURE;
	}

	add_teams_to_sort_list();
	sort();
	display_func();

	return EXIT_SUCCESS;
}
