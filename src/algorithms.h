#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include "results.h"

enum algorithm_id {
	ALGO_RPI = 1,
	ALGO_ELO
};

#define NUM_ALGOS 2

extern void algo_winper(void);
extern void algo_rpi(void);
extern void algo_elo(void);

struct algorithm_depends {
	const enum algorithm_id *depends;
	short num_depends;
};

struct algorithm {
	const char *name;
	enum algorithm_id algo;
	void (*hook)(int week, struct result*);
	struct algorithm_depends depends;

	struct algorithm *next;
};

extern struct algorithm algo_rpi_def;
extern struct algorithm algo_elo_def;

extern void (*algo_hooks[NUM_ALGOS])(int, struct result*);
extern void serialize_algos(void);

#endif
