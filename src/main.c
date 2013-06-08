
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "options.h"

struct elocfb_options options = {0};

static void set_algo_option(char *algo)
{
	if (strcmp(algo, "winper") == 0) {
		options.output_winper = true;
	} else if (strcmp(algo, "rpi") == 0) {
		options.output_rpi = true;
	} else if (strcmp(algo, "elo") == 0) {
		options.output_elo = true;
	} else {
		fprintf(stderr, "Unknown algorithm '%s'\n", algo);
		exit(EXIT_FAILURE);
	}
}

static void parse_algos(char *arg)
{
	char *token;

	token = strtok(arg, ",");

	while (token) {
		set_algo_option(token);
		token = strtok(NULL, ",");
	}
}

static int parse_long_opt(int argc, char **argv, int cur)
{
	char *arg = argv[cur] + 2;
	int skip = 0;

	if (strcmp(arg, "algo") == 0) {
		if (cur + 1 < argc) {
			parse_algos(argv[cur+1]);
			skip = 1;
		}
	} else {
		fprintf(stderr, "Unknown option '%s'\n", arg);
		exit(EXIT_FAILURE);
	}

	return skip;
}

static int parse_opt(int argc, char **argv, int cur)
{
	char *arg = argv[cur];
	int skip = 0;

	switch (arg[1]) {
	case 'n':
		options.output_rank = true;
		break;
	default:
		fprintf(stderr, "Unknown option '%s'\n", arg + 1);
		exit(EXIT_FAILURE);
	}

	return skip;
}

static void parse_opts(int argc, char **argv)
{
	int i;
	int skip;

	for (i = 1; i < argc; i++) {
		if (argv[i][0] == '-') {
			if (argv[i][1] == '-')
				skip = parse_long_opt(argc, argv, i);
			else
				skip = parse_opt(argc, argv, i);

			i += skip;
		}
	}
}

int main(int argc, char **argv)
{
	parse_opts(argc, argv);

	exit(EXIT_SUCCESS);
}
