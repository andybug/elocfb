
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <regex.h>

#include "parse.h"

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

int parse_ncaa(const char *file)
{
	char buf[512];
	FILE *stream;
	int failure;
	int records = 0;

	size_t len;
	regmatch_t matches[9];

	precompile();

	stream = fopen(file, "r");
	if (!stream) {
		fprintf(stderr, "couldn't open '%s'\n", file);
		return 1;
	}

	while (fgets(buf, 512, stream)) {
		failure = regexec(&result_regex, buf, 9, matches, 0);
		if (!failure)
			records++;
	}

	printf("found %d records\n", records);
	fclose(stream);
	return 0;
}
