/* drink.h — nd-drink's cross-module API: consumables and the consume hook.
 *
 * Caller-facing header. Implementers include <nd/drink-types.h>, not this header.
 */

#ifndef ND_DRINK_H
#define ND_DRINK_H

#include <ttypt/xy.h>
#include <nd/drink-types.h>

XY_DECL(int, on_consume, unsigned, player_ref, unsigned, vial_ref);

#endif /* !ND_DRINK_H */
