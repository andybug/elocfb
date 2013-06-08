
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "options.h"

struct elocfb_options options = {0};

struct arguments {
	int argc;
	char **argv;
	int current;
};

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

static void parse_algo_value(char *arg)
{
	char *token;

	token = strtok(arg, ",");

	while (token) {
		set_algo_option(token);
		token = strtok(NULL, ",");
	}
}

static char *get_opt_value(struct arguments *args)
{
	char *val = NULL;
	char *opt;

	if (args->current + 1 < args->argc) {
		args->current++;
		val = args->argv[args->current];
	} else {
		opt = args->argv[args->current];
		fprintf(stderr, "Option '%s' expected value\n", opt);
		exit(EXIT_FAILURE);
	}

	return val;

}

static void parse_long_opt(struct arguments *args)
{
	char *arg = args->argv[args->current] + 2;
	char *value;

	if (strcmp(arg, "algo") == 0) {
		value = get_opt_value(args);
		parse_algo_value(value);
	} else {
		fprintf(stderr, "Unknown option '%s'\n", arg);
		exit(EXIT_FAILURE);
	}
}

static void parse_opt(struct arguments *args)
{
	char *arg = args->argv[args->current];

	switch (arg[1]) {
	case 'n':
		options.output_show_rank = true;
		break;
	default:
		fprintf(stderr, "Unknown option '%s'\n", arg + 1);
		exit(EXIT_FAILURE);
	}
}

static void parse_opts(int argc, char **argv)
{
	struct arguments args = { argc, argv, 1 };

	for (; args.current < argc; args.current++) {
		if (argv[args.current][0] == '-') {
			if (argv[args.current][1] == '-')
				parse_long_opt(&args);
			else
				parse_opt(&args);
		}
	}
}

int main(int argc, char **argv)
{
	parse_opts(argc, argv);

	exit(EXIT_SUCCESS);
}
