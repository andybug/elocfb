
#define _XOPEN_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
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
		team = create_team(result->home_key);
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

int parse_ncaa(const char *file)
{
	char buf[512];
	FILE *stream;
	int failure;
	int records = 0;
	regmatch_t matches[9];

	precompile();

	stream = fopen(file, "r");
	if (!stream) {
		fprintf(stderr, "couldn't open '%s'\n", file);
		return 1;
	}

	while (fgets(buf, 512, stream)) {
		failure = regexec(&result_regex, buf, 9, matches, 0);
		if (!failure) {
			records++;
			parse_record(buf, matches);
		}
	}

	printf("found %d records\n", records);
	printf("found %d unique records\n", num_results);
	printf("found %d teams\n", num_teams);
	fclose(stream);
	return 0;
}
