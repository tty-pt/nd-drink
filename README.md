# axil-nd-drink

`nd-drink` for [axil-nd](../axil-nd), ported from SIC to libxylem.

Owns consumables: the `consumable_t` / `consumable_skel_t` types in
`<nd/drink.h>` that nd-plant's carrot and tomato (and any future cooking
module) build against, the `on_consume` hook they fire, and the drink
display path (`on_view_flags`, `on_add`).

## Install

```sh
make install
```

Installs:

```
lib/libnd-drink.so
include/nd/drink.h
```

There is deliberately no `lib/nd-drink.so` symlink (see `axil-nd-wts` for
why: `mods.load` names the installed filename, and the OpenBSD packing list
never lists a symlink).

Also packaged for deb, apk, rpm, brew and openbsd from a `v*` tag.

## Build from source

```sh
make
```

Needs [libxylem](https://github.com/tty-pt/libxylem) and the engine's game
API, `<nd/xy.h>`, plus `<nd/mortal.h>` and `<nd/core.h>` — from checkouts
beside this repo or from installed packages:

```sh
git clone https://github.com/tty-pt/nd-drink && cd nd-drink
git clone https://github.com/tty-pt/axil-nd ../axil-nd
git clone https://github.com/tty-pt/nd-mortal ../axil-nd-mortal
git clone https://github.com/tty-pt/nd-core ../axil-nd-core
make
```

Both the checkout `-I` flags and the installed-package paths are on the
command line at once (see `Makefile`), and a missing `-I` is ignored, so the
same command works either way. CI names the deps explicitly
(`axil-nd,libxylem,nd-mortal,nd-core`).

## What it does

* `consumable_t` (food, drink, quantity, capacity) and `consumable_skel_t`
  (food, drink) live in `<nd/drink.h>` because nd-plant reads them.
* `on_consume` is `XY_DECL`'d there: fired, but with no implementor anywhere
  the dispatch returns 0.
* `on_add` and `on_view_flags` cover creation and the drink display, and the
  old `on_icon` body is now a `core_icon_decorate` decorator registered in
  `xy_install` (the shop precedent).

## Testing

There is no `test.sh` here. Behaviour is asserted by the engine's own suite:

```sh
cd ../axil-nd
make && ./test.sh
```

## Notes from the port

* `SIC_DEF` → `XY_IMPL`, `mod_install` → `xy_install`, `call_f(...)` →
  `f(...)`.
* It opens no tables of its own in `xy_install` — the consumable rows live
  in the engine's skeleton/drop tables.
* The link line is libxylem alone. `NEEDED` is `libxylem.so` and `libc.so.6`.

## License

BSD 2-Clause, carried over from `tty-pt/nd-drink`. See `LICENSE`.
