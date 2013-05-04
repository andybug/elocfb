
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "commands.h"

struct cmd_struct {
	const char *name;
	int (*func)(int, const char**);
};

static const struct cmd_struct commands[] = {
	{ "init",   cmd_init },
	{ NULL,	    NULL     }
};

static const struct cmd_struct *find_command(const char *name)
{
	const struct cmd_struct *cmd = NULL;

	cmd = commands;
	while (cmd->name) {
		if (strcmp(cmd->name, name) == 0)
			return cmd;
		cmd++;
	}

	return NULL;
}

static int run_command(int argc, char **argv)
{
	struct cmd_struct *cmd;

	cmd = find_command(argv[1]);
	if (!cmd) {
		fprintf(stderr, "unknown command '%s'\n", argv[1]);
		exit(EXIT_FAILURE);
	}

	return cmd->func(argc, argv);
}

int main(int argc, char **argv)
{
	int err = 0;

	printf("argb[0] = %s\n", argv[0]);

	if (argc > 1)
		err = run_command(argc, argv);
	else
		exit(EXIT_FAILURE);

	exit(err);
}
