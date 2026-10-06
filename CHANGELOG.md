## 1.0.7

- **The sibling `-I` lines are gone.** `CFLAGS += -I$(shell cd .. && pwd)/…`
  pointed at the axil-nd/nd sibling checkouts and only existed for a dev
  build: in CI those directories do not exist and every header comes from the
  installed packages named in `.github/workflows/ci.yml`. The build now
  resolves `<nd/…>` the way a packager sees it.
- **macOS: link with `-undefined dynamic_lookup`.** macOS `ld` rejects
  undefined symbols in a shared library, but `WARN` needs `qsyslog` — an
  engine-provided function pointer resolved at `dlopen` time (Linux allows
  this by default). `-undefined dynamic_lookup` is the Darwin equivalent, set
  as `LDFLAGS-libnd-drink-Darwin` so no other platform is affected.

## [1.0.0]

- **nd-drink is now an installable library rather than a build artifact of
  the engine.** It builds and installs exactly two files,
  `lib/libnd-drink.so` and `include/nd/drink.h`, following the same layout as
  `axil-tty` and `axil-auth`, and the same layout `nd-core` was converted to
  first. Previously `make` produced a `drink.so` named by the engine's
  `mods.load` and installed nothing. There is no `lib/nd-drink.so` symlink:
  `mods.load` names this module `libnd-drink`, the installed filename, and
  `module_load_path()` only appends `.so`.

- **The link line is libxylem alone.** `LDLIBS := -lxylem`; the engine is not
  linked. `NEEDED` is `libxylem.so` and `libc.so.6`.

- **Consumables are declared in `<nd/drink.h>`.** `consumable_t` and
  `consumable_skel_t` live there because nd-plant (carrot, tomato) reads
  them; `on_consume` is `XY_DECL`'d there — fired, but with no implementor
  anywhere the dispatch returns 0.

- **The old `on_icon` body is now a `core_icon_decorate` decorator**,
  registered in `xy_install` (the shop precedent).

- **Dropped the `nd-mod.mk` dependency.** `nd-mod.mk` has now been deleted.
