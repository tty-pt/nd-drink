#include "./include/uapi/drink.h"

#include <stdio.h>

#include <nd/nd.h>
#include <nd/mortal.h>

#define DRINK_VALUE (1 << 14)
#define FOOD_VALUE(x) (1 << (16 - x->food))

unsigned act_fill, act_drink, act_eat,
	 type_consumable, vtf_pond;

SIC_DEF(int, on_consume, unsigned, player_ref, unsigned, vial_ref);

void
do_consume(int fd, int argc __attribute__((unused)), char *argv[]) {
	unsigned player_ref = fd_player(fd), vial_ref;
	char *name = argv[1];

	if (!*name || (vial_ref = ematch_mine(player_ref, name)) == NOTHING) {
		nd_writef(player_ref, NOMATCH_MESSAGE);
		return;
	}

	OBJ vial;
	nd_get(HD_OBJ, &vial, &vial_ref);

	if (vial.type != type_consumable) {
		nd_writef(player_ref, CANTDO_MESSAGE);
		return;
	}

	consumable_t *cvial = (consumable_t *) &vial.data;

	if (!cvial->quantity) {
		nd_writef(player_ref, "%s is empty.\n", vial.name);
		return;
	}

	call_feed(player_ref, cvial->drink ? DRINK_VALUE : 0,
			cvial->food ? FOOD_VALUE(cvial) : 0);

	cvial->quantity--;
	OBJ player;
	nd_get(HD_OBJ, &player, &player_ref);
	nd_owritef(player_ref, "%s consumes %s\n", player.name, vial.name);

	call_on_consume(player_ref, vial_ref);

	if (!cvial->quantity && !cvial->capacity)
		object_move(vial_ref, NOTHING);
	else
		nd_put(HD_OBJ, &vial_ref, &vial);
}

void
do_fill(int fd, int argc __attribute__((unused)), char *argv[])
{
	unsigned player_ref = fd_player(fd),
		 vial_ref = ematch_mine(player_ref, argv[1]),
		 source_ref = ematch_near(player_ref, argv[2]);

	if (vial_ref == NOTHING || source_ref == NOTHING) {
		nd_writef(player_ref, NOMATCH_MESSAGE);
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

	OBJ player;
	nd_get(HD_OBJ, &player, &player_ref);
	nd_owritef(player_ref, "%s fills %s from %s\n", player.name, vial.name, source.name);
	return;
error:
	nd_writef(player_ref, CANTDO_MESSAGE);
}

struct icon on_icon(struct icon i, unsigned ref, unsigned type)
{
	OBJ obj;
	consumable_t *cwhat;

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

int on_add(unsigned ref, unsigned type, uint64_t v __attribute__((unused)))
{
	OBJ obj;
	SKEL skel;
	consumable_t *cnu = (consumable_t *) &obj.data;

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

unsigned short on_view_flags(unsigned short flags, unsigned ref)
{
	OBJ obj;

	nd_get(HD_OBJ, &obj, &ref);
	if (obj.type != type_consumable)
		return flags;

	return flags | vtf_pond;
}

void mod_open(void *arg __attribute__((unused))) {
	vtf_pond = vtf_register('~', BLUE, BOLD);
	act_fill = action_register("fill", "💧");
	act_drink = action_register("drink", "🧪");
	act_eat = action_register("eat", "🥄");

	type_consumable = nd_put(HD_TYPE, NULL, "consumable");

	nd_register("consume", do_consume, 0);
	nd_register("fill", do_fill, 0);
}

void mod_install(void *arg) {
	mod_open(arg);
}
