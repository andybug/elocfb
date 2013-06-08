#ifndef OPTIONS_H
#define OPTIONS_H

#include <stdbool.h>

#define DATABASE_NAME_MAX	128
#define DATABASE_USER_MAX	128
#define DATABASE_PASSWORD_MAX	64

struct elocfb_options {
	/* output */
	bool output_winper;
	bool output_rpi;
	bool output_elo;
	bool output_show_rank;

	/* database */
	bool save_to_db;
	char dbname[DATABASE_NAME_MAX];
	char dbuser[DATABASE_USER_MAX];
	char dbpassword[DATABASE_PASSWORD_MAX];
};

extern struct elocfb_options options;

#endif
