
#include <stdio.h>

#include "../algorithms.h"
#include "../teams.h"
#include "../results.h"

void algo_winper(void)
{
	struct team *t1, *t2;
	struct result *result;
	int i;

	for (i = 0; i < num_results; i++) {
		result = results + i;

		t1 = find_team(&team_map, result->home_key);
		t2 = find_team(&team_map, result->away_key);

		if (result->home_pts > result->away_pts) {
			if (t1)
				t1->wins++;

			if (t2)
				t2->losses++;
		} else {
			if (t1)
				t1->losses++;

			if (t2)
				t2->wins++;
		}
	}

	for (i = 0; i < num_teams; i++)
		teams[i].winper = (float)teams[i].wins /
				  (teams[i].wins + teams[i].losses);
}
