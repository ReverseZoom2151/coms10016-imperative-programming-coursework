# C23 Systems Toolkit

A small, portable C23 toolkit for working close to the machine: fixed-width
binary representations, cursor-based circular lists, and compact binary
sketch streams. It is a library first, with focused command-line tools for the
operations that are useful from a terminal.

```text
binary-tool ── exact two's-complement conversion
int_list     ── reusable circular doubly linked-list library
sketch-view  ── compact sketch stream → ASCII preview
sketch-pgm   ── compact sketch stream → PGM image
```

## Quick start

Requirements: a C23-capable compiler, CMake 3.20+, and optionally Ninja.
Valgrind is used by the full verification target.

```sh
cmake -S . -B build -G Ninja
cmake --build build
ctest --test-dir build --output-on-failure
```

Or use the included Make targets:

```sh
make build   # configure and compile
make test    # run regression tests
make check   # tests plus Valgrind leak/error checks
```

## Binary conversion

`binary-tool` treats a value as an explicit-width integer rather than relying
on platform-dependent `int` or `long` sizes. Signed values use two's-complement
encoding; invalid digits and out-of-range values are rejected.

```sh
$ ./build/binary-tool encode i8 -12
11110100

$ ./build/binary-tool decode i8 11110100
-12

$ ./build/binary-tool encode u16 513
0000001000000001
```

Supported types are `i8`, `u8`, `i16`, `u16`, `i32`, `u32`, `i64`, and `u64`.
The public API lives in [`include/binary.h`](include/binary.h).

## Circular list library

`int_list` is an opaque, dynamically allocated circular doubly linked list.
Its cursor is either on an item or at the end sentinel, making boundary states
explicit and safe to query. Insertion moves the cursor to the inserted item;
erasure can move to either adjacent item.

```c
int_list *items = int_list_create();
int_list_insert_after(items, 20);
int_list_insert_before(items, 10);

int value;
int_list_first(items);       // cursor → 10
int_list_next(items);        // cursor → 20
int_list_get(items, &value); // value == 20
int_list_destroy(items);
```

See [`include/list.h`](include/list.h) for the complete API and its cursor
semantics.

## Sketch tools

The sketch decoder reconstructs a compact byte stream without a graphical
framework. `sketch-view` renders a terminal preview; `sketch-pgm` produces a
portable grayscale image.

```sh
./build/sketch-view drawing.sk 80 40
./build/sketch-pgm drawing.sk drawing.pgm 320 240
```

Each stream byte stores a two-bit opcode and a six-bit operand. The decoder
supports lines, filled blocks, grayscale colour selection, target coordinates,
and frame/display markers. The exact, implementation-backed format is in
[`docs/SKETCH_FORMAT.md`](docs/SKETCH_FORMAT.md).

## Project layout

```text
app/        command-line entry points
include/    public library headers
src/        reusable implementations
test/       executable regression tests
docs/       architecture and file-format notes
resources/  retained source material
```

The project builds with `-Wall -Wextra -Wpedantic -Werror`. GitHub Actions runs
the regression suite, AddressSanitizer/UBSan, and Valgrind on every push and
pull request.

## License

No license has been selected yet. Add one before distributing the toolkit.
