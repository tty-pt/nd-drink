/* drink.h — nd-drink's cross-module API: consumables and the consume hook.
 *
 * consumable_t / consumable_skel_t are read by nd-plant (carrot, tomato) and
 * any future cooking module, so they live here rather than in the provider TU.
 * on_consume is fired but has no implementor in-tree.
 *
 * Include this from a module TU that reads consumables or fires on_consume,
 * and NOT from nd-drink's own src/libnd-drink.c without DRINK_IMPL: an
 * XY_IMPL and an XY_DECL of the same name in one TU collide.
 *
 * NOTE: this is a MODULE-OWNED header, not an engine one. The old location was
 * `include/uapi/drink.h`; the old `~/nd/module.mk` installed it as
 * `$(PREFIX)/include/nd/drink.h`, so `nd/` is this header's home and it is
 * installed here with `FOLDER := nd`.
 */

#ifndef ND_DRINK_H
#define ND_DRINK_H

#include <ttypt/xy.h>

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

#ifndef DRINK_IMPL

XY_DECL(int, on_consume, unsigned, player_ref, unsigned, vial_ref);

#endif /* !DRINK_IMPL */

#endif /* !ND_DRINK_H */