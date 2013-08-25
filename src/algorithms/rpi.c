
#include "../algorithms.h"
#include "../results.h"
#include "../teams.h"

void algo_rpi_hook(int week, struct result *result);
static const enum algorithm_id rpi_depends[] = {};

struct algorithm algo_rpi_def = {
	"rpi",
	ALGO_RPI,
	algo_rpi_hook,
	{rpi_depends, 0},
	NULL
};

void algo_rpi(void)
{
	struct team *t1, *t2;
	struct result *r;
	float owinper, oowinper;
	int i;

	for (i = 0; i < num_results; i++) {
		r = results + i;

		t1 = find_team(&team_map, r->home_key);
		t2 = find_team(&team_map, r->away_key);

		if (!t1 || !t2)
			continue;

		t1->owins += t2->wins;
		t1->olosses += t2->losses;

		t2->owins += t1->wins;
		t2->olosses += t1->losses;
	}

	for (i = 0; i < num_results; i++) {
		r = results + i;

		t1 = find_team(&team_map, r->home_key);
		t2 = find_team(&team_map, r->away_key);

		if (!t1 || !t2)
			continue;

		t1->oowins += t2->owins;
		t1->oolosses += t2->olosses;

		t2->oowins += t1->owins;
		t2->oolosses += t1->olosses;
	}

	for (i = 0; i < num_teams; i++) {
		owinper = (float)teams[i].owins /
		          (teams[i].owins + teams[i].olosses);
		oowinper = (float)teams[i].oowins /
		           (teams[i].oowins + teams[i].oolosses);
		teams[i].rpi = (0.25 * teams[i].winper) +
		               (0.50 * owinper) + (0.25 * oowinper);
	}

}

void algo_rpi_hook(int week, struct result *result)
{
}
