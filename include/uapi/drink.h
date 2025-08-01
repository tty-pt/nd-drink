#ifndef DRINK_H
#define DRINK_H

#include <nd/type.h>

/* DATA */

typedef struct {
	unsigned food;
	unsigned drink;
} consumable_skel_t;

typedef struct {
	unsigned food;
	unsigned drink;
	unsigned quantity;
	unsigned capacity;
} consumable_t;

/* SIC */

SIC_DECL(int, on_consume, unsigned, player_ref, unsigned, vial_ref);

#endif
