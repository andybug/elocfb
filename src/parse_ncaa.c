
/* need this for strptime() and setenv() */
#define _XOPEN_SOURCE	600

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "parse.h"
#include "results.h"
#include "teams.h"

/*
 * Specify the header columns. These are what we expect to be, in this order,
 * on the first line of the input file
 */
static const char *header_entries[] = {
	"Institution ID",
	"Institution",
	"Game Date",
	"Opponent ID",
	"Opponent Name",
	"Score For",
	"Score Against",
	"Location"
};

/* Indicies that correspond to the header_entries above */
#define FIELD_TEAM_KEY		0
#define FIELD_TEAM_NAME		1
#define FIELD_DATE		2
#define FIELD_OPP_KEY		3
#define FIELD_OPP_NAME		4
#define FIELD_TEAM_SCORE	5
#define FIELD_OPP_SCORE		6
#define FIELD_LOCATION		7
#define NUM_FIELDS 		8

/*
 * This map tracks which teams have had their schedules completely parsed. This
 * allows us to ignore records that have already been parsed.
 */
static struct team_map processed_teams;


static time_t parse_date(const char *str)
{
	struct tm tm;

	memset(&tm, 0, sizeof(struct tm));
	tm.tm_isdst = -1;

	strptime(str, "%m/%d/%y", &tm);

	return mktime(&tm);
}

static void set_tz_env(void)
{
	/*
	 * Ensure that the TZ environment variable is set, otherwise mktime
	 * will run really slow
	 */
	if (!getenv("TZ"))
		setenv("TZ", "America/Chicago", 0);
}

static void parse_record(char *tokens[])
{
	static int prev_key = 0;
	static struct team *team = NULL;
	struct result *result = results + num_results;
	int key, key2;
	time_t date;

	key = atoi(tokens[FIELD_TEAM_KEY]);
	key2 = atoi(tokens[FIELD_OPP_KEY]);
	date = parse_date(tokens[FIELD_DATE]);

	/*
	 * if team2's schedule has already been processed,
	 * ignore this record
	 */
	if (find_team(&processed_teams, key2))
		return;

	/*
	 * if we're starting to process a new team's schedule, then add
	 * the current team to the processed list and create the new team
	 */
	if (key != prev_key) {
		if (team)
			add_team(&processed_teams, team);

		team = teams + num_teams;
		team->key = key;
		strcpy(team->name, tokens[FIELD_TEAM_NAME]);
		add_team(&team_map, team);

		num_teams++;
		prev_key = key;
	}

	if (strcmp("Home", tokens[FIELD_LOCATION]) == 0) {
		result->home_key = key;
		result->away_key = key2;
		result->home_pts = (short) atoi(tokens[FIELD_TEAM_SCORE]);
		result->away_pts = (short) atoi(tokens[FIELD_OPP_SCORE]);
		result->neutral = false;
	} else if (strcmp("Away", tokens[FIELD_LOCATION]) == 0) {
		result->home_key = key2;
		result->away_key = key;
		result->home_pts = (short) atoi(tokens[FIELD_OPP_SCORE]);
		result->away_pts = (short) atoi(tokens[FIELD_TEAM_SCORE]);
		result->neutral = false;
	} else if (strcmp("Neutral Site", tokens[FIELD_LOCATION]) == 0) {
		result->home_key = key;
		result->away_key = key2;
		result->home_pts = (short) atoi(tokens[FIELD_TEAM_SCORE]);
		result->away_pts = (short) atoi(tokens[FIELD_OPP_SCORE]);
		result->neutral = true;
	}

	result->date = date;
	num_results++;
}

static int tokenize_line(char *line, char *tokens[])
{
	bool inside = false;
	int fields = 0;

	while (*line) {
		if (*line == '"') {
			if (inside) {
				*line = '\0';
				inside = false;
			} else {
				inside = true;
				tokens[fields] = line + 1;
				fields++;
			}
		}

		line++;
	}

	return fields;
}

static int parse_header(char *line)
{
	int i;
	int num_tokens;
	char *tokens[NUM_FIELDS];
	bool error = false;

	num_tokens = tokenize_line(line, tokens);

	if (num_tokens != NUM_FIELDS)
		error = true;

	for (i = 0; i < num_tokens && !error; i++) {
		if (strcmp(header_entries[i], tokens[i]) != 0)
			error = true;
	}

	return error;
}

int parse_ncaa(const char *file)
{
	char buf[512];
	FILE *stream;
	char *tokens[NUM_FIELDS];
	int num_tokens;
	int err;

	set_tz_env();

	init_teams();
	init_team_map(&team_map);
	init_team_map(&processed_teams);

	stream = fopen(file, "r");
	if (!stream) {
		fprintf(stderr, "%s: Couldn't open file '%s'\n", __func__, file);
		return 1;
	}

	/* read the heading line and ensure it's what we want */
	if (fgets(buf, 512, stream)) {
		err = parse_header(buf);
		if (err) {
			fprintf(stderr, "File '%s' does not match expected header form\n", file);
			return 1;
		}
	} else {
		fprintf(stderr, "Couldn't read from file '%s'\n", file);
		return 1;
	}

	while (fgets(buf, 512, stream)) {
		num_tokens = tokenize_line(buf, tokens);

		if (num_tokens != NUM_FIELDS) {
			fprintf(stderr, "Error parsing line '%s'\n", buf);
			return 1;
		}

		parse_record(tokens);
	}

	fclose(stream);
	return 0;
}
