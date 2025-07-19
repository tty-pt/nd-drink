#ifndef DRINK_H
#define DRINK_H

#include <nd/type.h>

extern unsigned type_consumable;

// skeleton
typedef struct {
	unsigned food;
	unsigned drink;
} SCON;

// instance
typedef struct {
	unsigned food;
	unsigned drink;
	unsigned quantity;
	unsigned capacity;
} CON;

SIC_DECL(int, on_consume, unsigned, player_ref, unsigned, vial_ref);

#endif
