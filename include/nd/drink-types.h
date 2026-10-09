#ifndef ND_DRINK_TYPES_H
#define ND_DRINK_TYPES_H

/*
 * nd/drink-types.h — Consumable structures for nd-drink and consumers.
 * Contains zero XY_DECLs.
 */

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

#endif /* ND_DRINK_TYPES_H */
