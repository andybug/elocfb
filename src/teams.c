
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "teams.h"

static const int mask = MAX_TEAMS - 1;

struct team teams[MAX_TEAMS];
int num_teams = 0;
struct team_map team_map;

void init_teams(void)
{
	memset(teams, 0, sizeof(struct team) * MAX_TEAMS);
	num_teams = 0;
}

void init_team_map(struct team_map *map)
{
	memset(map, 0, sizeof(struct team_map));
}

void add_team(struct team_map *map, struct team *team)
{
	int i = team->key & mask;
	int iters = 0;

	while (map->entries[i].key != 0) {
		if (iters >= num_teams) {
			fprintf(stderr, "%s: Team hash table full; could not "
				"add key '%d'\n", __func__, team->key);
			exit(EXIT_FAILURE);
		}

		i = (i + 1) & mask;
		iters++;
	}

	map->entries[i].key = team->key;
	map->entries[i].team = team;
}

struct team *find_team(struct team_map *map, int key)
{
	int i = key & mask;
	int iters = 0;

	while (map->entries[i].key != key) {
		if (map->entries[i].key == 0 || iters >= num_teams)
			return NULL;

		i = (i + 1) & mask;
		iters++;
	}

	return map->entries[i].team;
}
