
#include <stdlib.h>
#include <stdio.h>

#include "../commands.h"
#include "../parse.h"
#include "../teams.h"
#include "../results.h"

int cmd_parse(int argc, char **argv)
{
	if (argc != 3) {
		fprintf(stderr, "elocfb parse [file.csv]\n");
		return EXIT_FAILURE;
	}

	parse_ncaa(argv[2]);
	sort_results();

	printf("found %d unique records\n", num_results);
	printf("found %d teams\n", num_teams);

	return EXIT_SUCCESS;
}
