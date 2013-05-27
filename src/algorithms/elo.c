
#include <math.h>

#include "../algorithms.h"
#include "../teams.h"
#include "../results.h"

#define ELO_AVERAGE 1200.0

static void seed_elo(void)
{
	int i;

	for (i = 0; i < num_teams; i++)
		teams[i].elo = (short)(teams[i].rpi * 2.0 * ELO_AVERAGE);
}

void algo_elo(void)
{
	int i;
	struct team *t1, *t2;
	double exp1, exp2;
	double t1win;

	seed_elo();

	for (i = 0; i < num_results; i++) {
		t1 = find_team(&team_map, results[i].home_key);
		t2 = find_team(&team_map, results[i].away_key);

		if (!t1 || !t2)
			continue;

		exp1 = 1.0 / (1.0 + pow(10, (t2->elo - t1->elo) / 400.0));
		exp2 = 1.0 / (1.0 + pow(10, (t1->elo - t2->elo) / 400.0));

		if (results[i].home_pts > results[i].away_pts)
			t1win = 1.0;
		else
			t1win = 0.0;

		t1->elo += (short)(32.0 * (t1win - exp1));
		t2->elo += (short)(32.0 * ((t1win == 1.0 ? 0.0 : 1.0) - exp2));
	}
}
