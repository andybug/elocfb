#ifndef OPTIONS_H
#define OPTIONS_H

#include <stdbool.h>

struct elocfb_options {
	/* output */
	bool output_winper;
	bool output_rpi;
	bool output_elo;
	
	bool output_show_rank;
};

extern struct elocfb_options options;

#endif
