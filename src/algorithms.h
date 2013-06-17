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

struct algorithm {
	const char *name;
	enum algorithm_id algo;
	void (*hook)(int week, struct result*);
	enum algorithm_id deps[NUM_ALGOS];
	int num_deps;
	struct algorithm *next;
};

extern struct algorithm *algo_list;

extern void serialize_algos(void);

#endif
