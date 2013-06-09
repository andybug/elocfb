#ifndef ALGORITHMS_H
#define ALGORITHMS_H

enum algorithm {
	ALGO_RPI,
	ALGO_ELO
};

extern void algo_winper(void);
extern void algo_rpi(void);
extern void algo_elo(void);

#endif
