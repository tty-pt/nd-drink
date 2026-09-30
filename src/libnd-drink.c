/* src/libnd-drink.c — nd-drink, ported to libxylem.
 *
 * Owns consumables (food and drink): the consume/fill commands, the pond
 * view-flag, and the item definitions the client renders with drink/eat/fill
 * actions.
 *
 * Original: tty-pt/nd-drink @ 171 lines main.c, from the nd-basics
 * superproject.
 *
 * This TU XY_IMPLs on_add and on_view_flags. on_consume is fired
 * but has no implementor in-tree, so it stays XY_DECL-only (in nd/drink.h,
 * included here unguarded). The consumable_* types come from that header.
 *
 * nd_owritef() is variadic and cannot cross the bus; it ports as snprintf +
 * nd_rwrite, which is exactly what it did.
 *
 * on_icon does not exist here anymore. nd-drink amends icons through nd-core's
 * decorator table (core_icon_decorate), because XY cannot observe a
 * co-implemented chain; see <nd/core.h> and MODS.md §7.
 */

#include <ttypt/xy-mod.h>

#include <nd/xy.h>

#include <stdio.h>
#include <string.h>

#include <nd/drink.h>
#include <nd/mortal.h>
#include <nd/core.h>

#define DRINK_VALUE (1 << 14)
#define FOOD_VALUE(x) (1 << (16 - x->food))

static unsigned act_fill, act_drink, act_eat,
	 type_consumable, vtf_pond;

/* API. XY_IMPL both defines the function and emits the dispatch adapter, so
 * each name gets exactly one, with its body -- no forward declarations.
 *
 * Order matters below: XY_IMPL emits a definition, so a caller has to come
 * after its callee. */

static void
do_consume(int fd, int argc __attribute__((unused)), char *argv[]) {
	unsigned player_ref = fd_player(fd), vial_ref;
	char *name = argv[1];

	if (!*name || (vial_ref = ematch_mine(player_ref, name)) == NOTHING) {
		nd_printf(player_ref, "I don't know what you mean.\n");
		return;
	}

	OBJ vial;
	nd_get(HD_OBJ, &vial, &vial_ref);

	if (vial.type != type_consumable) {
		nd_printf(player_ref, "You can't do that.\n");
		return;
	}

	consumable_t *cvial = (consumable_t *) &vial.data;

	if (!cvial->quantity) {
		nd_printf(player_ref, "%s is empty.\n", vial.name);
		return;
	}

	feed(player_ref, cvial->drink ? DRINK_VALUE : 0,
			cvial->food ? FOOD_VALUE(cvial) : 0);

	cvial->quantity--;
	{
		OBJ player;
		char buf[BUFSIZ];
		int len;

		nd_get(HD_OBJ, &player, &player_ref);
		len = snprintf(buf, sizeof(buf), "%s consumes %s\n",
			player.name, vial.name);
		nd_rwrite(player.location, player_ref, buf, (size_t)len);
	}

	on_consume(player_ref, vial_ref);

	if (!cvial->quantity && !cvial->capacity)
		object_move(vial_ref, NOTHING);
	else
		nd_put(HD_OBJ, &vial_ref, &vial);
}

static void
do_fill(int fd, int argc __attribute__((unused)), char *argv[])
{
	unsigned player_ref = fd_player(fd),
		 vial_ref = ematch_mine(player_ref, argv[1]),
		 source_ref = ematch_near(player_ref, argv[2]);

	if (vial_ref == NOTHING || source_ref == NOTHING) {
		nd_printf(player_ref, "I don't know what you mean.\n");
		return;
	}

	OBJ vial;
	nd_get(HD_OBJ, &vial, &vial_ref);
	consumable_t *cvial = (consumable_t *) &vial.data;

	if (vial.type != type_consumable || !cvial->capacity)
		goto error;

	OBJ source;
	nd_get(HD_OBJ, &source, &source_ref);

	if (source.type != type_consumable)
		goto error;

	consumable_t *csource = (consumable_t *) &source.data;
	cvial->quantity = cvial->capacity;
	cvial->drink = csource->drink;
	cvial->food = csource->food;
	nd_put(HD_OBJ, &vial_ref, &vial);

	{
		OBJ player;
		char buf[BUFSIZ];
		int len;

		nd_get(HD_OBJ, &player, &player_ref);
		len = snprintf(buf, sizeof(buf), "%s fills %s from %s\n",
			player.name, vial.name, source.name);
		nd_rwrite(player.location, player_ref, buf, (size_t)len);
	}
	return;
error:
	nd_printf(player_ref, "You can't do that.\n");
}

XY_IMPL(int, on_add, unsigned, ref, unsigned, type, uint64_t, v)
{
	OBJ obj;
	SKEL skel;
	consumable_t *cnu = (consumable_t *) &obj.data;

	(void) v;
	if (type != type_consumable)
		return 1;

	nd_get(HD_OBJ, &obj, &ref);
	nd_get(HD_SKEL, &skel, &obj.skid);
	consumable_skel_t *scon = (consumable_skel_t *) &skel.data;
	cnu->food = scon->food;
	cnu->drink = scon->drink;

	nd_put(HD_OBJ, &ref, &obj);
	return 0;
}

XY_IMPL(unsigned short, on_view_flags, unsigned short, flags, unsigned, ref)
{
	OBJ obj;

	nd_get(HD_OBJ, &obj, &ref);
	if (obj.type != type_consumable)
		return flags;

	return flags | vtf_pond;
}

/* The on_icon co-implementation is now a decorator. nd-core owns on_icon and
 * runs the registered decorators in order; this one marks drinkables and
 * edibles with their actions and glyphs. Registered in xy_install below. */
static struct icon
drink_icon_decorate(struct icon i, unsigned ref, unsigned type,
	unsigned player_ref)
{
	OBJ obj;
	consumable_t *cwhat;

	(void) player_ref;
	if (type != type_consumable)
		return i;

	nd_get(HD_OBJ, &obj, &ref);
	cwhat = (consumable_t *) &obj.data;

	if (cwhat->drink) {
		i.actions |= act_fill;
		i.pi.fg = BLUE;
		i.ch = '~';
		if (cwhat->quantity)
			i.actions |= act_drink;
	} else {
		i.pi.fg = RED;
		i.ch = 'o';
		if (cwhat->quantity)
			i.actions |= act_eat;
	}

	i.actions |= act_drink;
	return i;
}

XY_MODULE_API void
xy_install(void)
{
	/* Order matches the original mod_install: opens first, then the type
	 * and actions. */
	vtf_pond = (unsigned)vtf_register('~', BLUE, BOLD);
	nd_register("consume", do_consume, 0);
	nd_register("fill", do_fill, 0);

	type_consumable = (unsigned)nd_put(HD_TYPE, NULL, "consumable");

	act_fill = (unsigned)action_register("fill", "💧");
	act_drink = (unsigned)action_register("drink", "🧪");
	act_eat = (unsigned)action_register("eat", "🥄");

	core_icon_decorate(drink_icon_decorate);
}