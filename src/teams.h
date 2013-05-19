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
};

extern struct team teams[MAX_TEAMS];
extern int num_teams;

void init_teams(void);
struct team *create_team(int key);
struct team *find_team(int key);

#endif
