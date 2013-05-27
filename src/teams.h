#ifndef TEAMS_H
#define TEAMS_H

#define MAX_TEAMS 256
#define TEAMNAME_MAX 128

struct team {
	/* don't change the key */
	int key;

	char name[TEAMNAME_MAX];

	unsigned char wins;
	unsigned char losses;
	float winper;

	short owins;
	short olosses;
	short oowins;
	short oolosses;
	float rpi;

	short elo;
};

struct team_map_entry {
	int key;
	struct team *team;
};

struct team_map {
	struct team_map_entry entries[MAX_TEAMS];
};

extern struct team teams[MAX_TEAMS];
extern int num_teams;

extern struct team_map team_map;

void init_teams(void);
void init_team_map(struct team_map *map);
void add_team(struct team_map *map, struct team *team);
struct team *find_team(struct team_map *map, int key);

#endif
