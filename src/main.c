
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "options.h"
#include "parse.h"
#include "results.h"
#include "algorithms.h"
#include "database.h"
#include "output.h"

struct elocfb_options options;

struct arguments {
	int argc;
	char **argv;
	int current;
	char *file;
};

static const char *usage =
	"usage: elocfb [--help] [--version] [--algo <algorithms>] [-n] <file>\n"
	"\t--help      Display this help message\n"
	"\t--version   Print version and exit\n"
	"\t--algo      Select which algorithms to display/save. Can be any\n"
	"\t            combination of winper, rpi, and elo; comma separated\n"
	"\t            The first algorithm listed is used for sorting\n"
	"\t            For instance: --algo rpi,elo\n"
	"\t--database  Specify a database to save output to instead of stdout\n"
	"\t--user      Specify the user to use when logging in to the database\n"
	"\t-n          Display rank indicator in output\n";

static void init_options(void)
{
	memset(&options, 0, sizeof(struct elocfb_options));

	options.output_elo = true;
	options.output_sort_algo = ALGO_ELO;

	strcpy(options.dbname, "elocfb");
	strcpy(options.dbuser, "elocfb");
	strcpy(options.dbpassword, "elocfb");
}

static void set_algo_option(char *algo, bool is_first)
{
	if (strcmp(algo, "winper") == 0) {
		options.output_winper = true;
	} else if (strcmp(algo, "rpi") == 0) {
		options.output_rpi = true;
		if (is_first)
			options.output_sort_algo = ALGO_RPI;
	} else if (strcmp(algo, "elo") == 0) {
		options.output_elo = true;
		if (is_first)
			options.output_sort_algo = ALGO_ELO;
	} else {
		fprintf(stderr, "Unknown algorithm '%s'\n", algo);
		exit(EXIT_FAILURE);
	}
}

static void parse_algo_value(char *arg)
{
	char *token;
	bool is_first = true;

	token = strtok(arg, ",");

	while (token) {
		set_algo_option(token, is_first);
		token = strtok(NULL, ",");
		is_first = false;
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

	if (strcmp(arg, "help") == 0) {
		puts(usage);
		exit(EXIT_SUCCESS);
	} else if (strcmp(arg, "algo") == 0) {
		/* since algo list is provided, reset defaults */
		options.output_elo = false;

		value = get_opt_value(args);
		parse_algo_value(value);
	} else if (strcmp(arg, "database") == 0) {
		value = get_opt_value(args);
		strcpy(options.dbname, value);
		options.save_to_db = true;
	} else if (strcmp(arg, "user") == 0) {
		value = get_opt_value(args);
		strcpy(options.dbuser, value);
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

static void parse_opts(struct arguments *args)
{
	for (args->current = 1; args->current < args->argc; args->current++) {
		if (args->argv[args->current][0] == '-') {
			if (args->argv[args->current][1] == '-')
				parse_long_opt(args);
			else
				parse_opt(args);
		} else {
			if (args->file == NULL)
				args->file = args->argv[args->current];
		}
	}

	if (!args->file) {
		fprintf(stderr, "No file specified!\n");
		exit(EXIT_FAILURE);
	}
}

int main(int argc, char **argv)
{
	struct arguments args = { argc, argv, 0, NULL };
	int err;

	if (argc == 1) {
		puts(usage);
		exit(EXIT_SUCCESS);
	}

	init_options();
	parse_opts(&args);

	parse_ncaa(args.file);
	sort_results();

	algo_winper();
	algo_rpi();
	algo_elo();

	if (options.save_to_db) {
		err = db_connect();
		if (err)
			exit(EXIT_FAILURE);

		db_add_teams();
		db_disconnect();
	} else
		output_to_stdout();

	exit(EXIT_SUCCESS);
}
