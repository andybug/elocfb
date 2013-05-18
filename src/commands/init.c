
#include <stdio.h>
#include <limits.h>
#include <unistd.h>

#include "../commands.h"

int cmd_init(int argc, char **argv)
{
	char cwd[PATH_MAX];

	getcwd(cwd, PATH_MAX);
	printf("cmd_init: running from %s\n", cwd);
	return 0;
}
