
#define _XOPEN_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <regex.h>

#include "parse.h"
#include "results.h"
#include "teams.h"

static regex_t result_regex;
static const char *result_pattern =
	"^\"([0-9]+)\","			/* 1. team id */
	"\"([A-Za-z.&-\\(\\) ]+)\","		/* 2. team name */
	"\"([0-9]{2}/[0-9]{2}/[0-9]{2})\","	/* 3. date */
	"\"([0-9]+)\","				/* 4. opp id */
	"\"([A-Za-z.&-\\(\\) ]+)\","		/* 5. opp name */
	"\"([0-9]+)\","				/* 6. team score */
	"\"([0-9]+)\","				/* 7. opp score */
	"\"(Away|Home|Neutral Site)\"\n$";	/* 8. location */

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

#define NUM_FIELDS 8

static void precompile(void)
{
	int err;

	err = regcomp(&result_regex, result_pattern, REG_EXTENDED);
	if (err) {
		fprintf(stderr, "%s: error compiling regex\n", __func__);
		exit(EXIT_FAILURE);
	}
}

static int match_to_int(const char *str, regmatch_t *match)
{
	char buf[64];
	size_t len;

	len = match->rm_eo - match->rm_so;
	strncpy(buf, str + match->rm_so, len);
	buf[len] = '\0';

	return atoi(buf);
}

static time_t parse_date(const char *str, regmatch_t *match)
{
	char buf[64];
	size_t len;
	struct tm tm;

	memset(&tm, 0, sizeof(struct tm));

	len = match->rm_eo - match->rm_so;
	strncpy(buf, str + match->rm_so, len);
	buf[len] = '\0';

	strptime(buf, "%m/%d/%y", &tm);
	return mktime(&tm);
}

static void parse_team(const char *str, regmatch_t *match, char *name)
{
	size_t len;

	len = match->rm_eo - match->rm_so;
	strncpy(name, str + match->rm_so, len);
	name[len] = '\0';
}

static void parse_record(const char *str, regmatch_t *matches)
{
	static int prev_key = 0;
	struct result *result = results + num_results;
	struct team *team;
	int key, key2;
	time_t date;

	key = match_to_int(str, matches + 1);
	key2 = match_to_int(str, matches + 4);
	date = parse_date(str, matches + 3);

	if (result_exists(key, key2, date))
		return;

	if (key != prev_key) {
		team = create_team(key);
		parse_team(str, matches + 2, team->name);
		num_teams++;
		prev_key = key;
	}

	if (strncmp("Home", str + matches[8].rm_so, 4) == 0) {
		result->home_key = key;
		result->away_key = key2;
		result->home_pts = (short) match_to_int(str, matches + 6);
		result->away_pts = (short) match_to_int(str, matches + 7);
		result->neutral = false;
	} else if (strncmp("Away", str + matches[8].rm_so, 4) == 0) {
		result->home_key = key2;
		result->away_key = key;
		result->home_pts = (short) match_to_int(str, matches + 7);
		result->away_pts = (short) match_to_int(str, matches + 6);
		result->neutral = false;
	} else if (strncmp("Neutral Site", str + matches[8].rm_so, 12) == 0) {
		result->home_key = key;
		result->away_key = key2;
		result->home_pts = (short) match_to_int(str, matches + 6);
		result->away_pts = (short) match_to_int(str, matches + 7);
		result->neutral = true;
	}

	result->date = date;
	num_results++;
}

static int tokenize_line(char *line, char *tokens[NUM_FIELDS])
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
	int failure;
	char *tokens[NUM_FIELDS];
	int err;

	precompile();

	stream = fopen(file, "r");
	if (!stream) {
		fprintf(stderr, "Couldn't open '%s'\n", file);
		return 1;
	}

	/* read the heading line and ensure it's what we want */
	fgets(buf, 512, stream);
	err = parse_header(buf);
	if (err) {
		fprintf(stderr, "File '%s' does not match expected header form\n", file);
		return 1;
	}

	/* while (fgets(buf, 512, stream)) {
	} */

	fclose(stream);
	return 0;
}
