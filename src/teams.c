
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "teams.h"

static const int mask = MAX_TEAMS - 1;

struct team teams[MAX_TEAMS];
int num_teams = 0;

void init_teams(void)
{
	memset(teams, 0, sizeof(struct team) * MAX_TEAMS);
	num_teams = 0;
}

struct team *create_team(int key)
{
	int i = key & mask;
	int iters = 0;

	while (teams[i].key != 0) {
		if (iters >= num_teams) {
			fprintf(stderr, "%s: Team hash table full; could not add key '%d'\n", __func__, key);
			exit(EXIT_FAILURE);
		}

		i = (i+1) & mask;
		iters++;
	}

	teams[i].key = key;
	return (teams + i);
}

struct team *find_team(int key)
{
	int i = key & mask;
	int iters = 0;

	while (teams[i].key != key) {
		if (iters >= num_teams)
			return NULL;

		i = (i+1) & mask;
		iters++;
	}

	return (teams + i);
}
