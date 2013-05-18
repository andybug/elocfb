
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

static void parse_record(const char *str, regmatch_t *matches)
{
	struct result *result = results + num_results;
	int home_key;
	int away_key;

	if (strncmp("Home", str + matches[8].rm_so, 4) == 0) {
		result->home_key = match_to_int(str, matches + 1);
		result->away_key = match_to_int(str, matches + 4);
		result->home_pts = (short) match_to_int(str, matches + 6);
		result->away_pts = (short) match_to_int(str, matches + 7);
		result->neutral = false;
		result->date = parse_date(str, matches + 3);

		num_results++;
	} else if (strncmp("Neutral Site", str + matches[8].rm_so, 12) == 0) {
		home_key = match_to_int(str, matches + 1);
		away_key = match_to_int(str, matches + 4);

		if (result_exists(home_key, away_key))
			return;

		result->home_key = home_key;
		result->away_key = away_key;
		result->home_pts = (short) match_to_int(str, matches + 6);
		result->away_pts = (short) match_to_int(str, matches + 7);
		result->neutral = true;
		result->date = parse_date(str, matches + 3);

		num_results++;
	}
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
	fclose(stream);
	return 0;
}
