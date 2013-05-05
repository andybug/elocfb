
#include <stdlib.h>
#include <stdio.h>

#include "../commands.h"
#include "../parse.h"

int cmd_parse(int argc, char **argv)
{
	if (argc != 3) {
		fprintf(stderr, "shiny-avenger parse [file.csv]\n");
		return EXIT_FAILURE;
	}

	printf("parsing '%s'\n", argv[2]);
	parse_ncaa(argv[2]);

	return EXIT_SUCCESS;
}
