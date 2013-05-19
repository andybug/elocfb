
#include <stdio.h>
#include <stdlib.h>

#include "../commands.h"
#include "../parse.h"
#include "../teams.h"
#include "../results.h"
#include "../algorithms.h"

int cmd_process(int argc, char **argv)
{
	if (argc != 3) {
		fprintf(stderr, "shiny-avenger process [file.csv]\n");
		return EXIT_FAILURE;
	}

	init_teams();
	parse_ncaa(argv[2]);
	sort_results();

	algo_winper();

	return EXIT_SUCCESS;
}
