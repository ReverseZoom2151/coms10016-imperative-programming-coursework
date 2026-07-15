<div align="center">

# C23 Systems Toolkit

### A portable C23 CLI toolkit for exact binary data, cursor-based containers, and compact raster sketches.

[![CI](https://github.com/ReverseZoom2151/c23-systems-toolkit/actions/workflows/ci.yml/badge.svg)](https://github.com/ReverseZoom2151/c23-systems-toolkit/actions/workflows/ci.yml)

</div>

`c23-systems-toolkit` is a buildable C23 project with three focused systems
programming domains. Each has a small public API, a terminal-friendly boundary,
and executable regression tests. The project treats invalid input, integer
widths, memory ownership, and file output as explicit parts of its contract.

| Domain | What you can do | Systems idea |
| --- | --- | --- |
| **Binary** | Encode, decode, group, inspect, and convert exact 8-, 16-, 32-, and 64-bit values to hexadecimal | Two's-complement representation and range checking |
| **Lists** | Reuse integer or generic ownership-aware cursor-based circular lists | Sentinel nodes, pointer invariants, and ownership |
| **Sketches** | Inspect compact byte streams, render terminal previews, or export scaled/inverted P2/P5 PGM files | Bit-packed commands, rasterisation, clipping, and file I/O |

## Run it

You need a C23-capable compiler, [CMake](https://cmake.org/) 3.20+, and
optionally [Ninja](https://ninja-build.org/). [Valgrind](https://valgrind.org/)
is used by the complete local verification target.

```bash
git clone https://github.com/ReverseZoom2151/c23-systems-toolkit.git
cd c23-systems-toolkit
cmake -S . -B build -G Ninja
cmake --build build
ctest --test-dir build --output-on-failure
```

Convert a signed value to its exact two's-complement form, then decode it:

```bash
./build/binary-tool encode i8 -12
# 11110100

./build/binary-tool decode i8 11110100
# -12

./build/binary-tool encode --explain i8 -12
# type: i8
# binary: 1111_0100
# hexadecimal: 0xF4
# two's-complement interpretation: -12
```

Use the sketch tools on a compact `.sk` stream. Fixtures are included under
[`examples/`](examples/), so these commands work directly after building:

```bash
./build/sketch-view examples/line.sk 5 5
./build/sketch-inspect examples/line.sk
./build/sketch-pgm examples/line.sk line.pgm --plain --invert --scale 2 5 5
```

The generated PGM is deliberately simple and portable: it can be inspected by
image tools, converted elsewhere, or treated as raw grayscale output. The
plain `P2` form is useful for reviews and diffs; binary `P5` is compact.

## Why these modules belong together

The executables are intentionally thin. Parsing and terminal/file I/O live at
the edge; reusable code in `src/` exposes explicit values and status results.
That makes fixed-width behaviour testable, pointer ownership local, and binary
formats usable without a GUI framework.

For example, the binary API takes an explicit bit width and reports why a
request could not be represented instead of silently truncating a value:

```c
binary_status binary_encode_signed(
    int64_t value, unsigned bits, char *output, size_t output_size);
```

The list remains opaque to callers. Its cursor is either on an item or at an
end sentinel, so navigation and mutation have observable, safe boundary
states:

```c
int_list *items = int_list_create();
int_list_insert_after(items, 20);
int_list_insert_before(items, 10);

int_list_first(items);       // cursor → 10
int_list_next(items);        // cursor → 20
int_list_destroy(items);
```

For callers with their own data types, `generic_list` has the same cursor model
but stores `void *` values. A destructor supplied at creation owns remaining
values, while `generic_list_release_current` explicitly transfers ownership
back to the caller.

The sketch decoder has the same compact boundary: each input byte is a two-bit
opcode plus a six-bit operand. `DATA` accumulates parameters; movement and
tool commands consume them to produce clipped line or block raster operations.

```c
sketch_status sketch_decode_bytes(
    sketch_canvas *canvas, const uint8_t *bytes, size_t length);
```

This keeps the format decoder independent from the output choice: the same
canvas can be written as ASCII or binary PGM.

## Rendered examples

This image is a real SVG export generated from
[`examples/gallery.sk`](examples/gallery.sk), not a mockup. It combines a
grayscale panel, a block-built **C**, and a rasterised chevron; SVG scales those
exact pixels for readable repository previewing.

<p align="center">
  <img src="examples/gallery.svg" alt="A grayscale C23 Systems Toolkit gallery image rendered from a binary sketch fixture" width="640" />
</p>

Regenerate it from the committed binary fixture:

```bash
./build/sketch-svg examples/gallery.sk examples/gallery.svg --scale 20 32 20
```

The same decoder also writes a looping GIF from complete sketch frames. This
animation builds the gallery panel, the block-based **C**, then the chevron:

<p align="center">
  <img src="examples/gallery.gif" alt="Animated gallery assembled from three decoded binary sketch frames" width="640" />
</p>

```bash
./build/sketch-gif examples/gallery.gif examples/gallery-frame-1.sk \
  examples/gallery-frame-2.sk examples/gallery.sk --delay 45 32 20
```

Run `sketch-inspect` to see the corresponding instruction trace, including the
opcode byte, accumulated data, cursor/target movement, drawing tool, colour,
and frame number. Golden ASCII, P2 PGM, SVG, and trace outputs live alongside
the fixture in [`examples/`](examples/).

## Command reference

```text
binary-tool encode|decode [--hex] [--group] [--explain] TYPE VALUE
sketch-view INPUT.sk [WIDTH HEIGHT]
sketch-inspect INPUT.sk
sketch-pgm INPUT.sk OUTPUT.pgm [--plain] [--invert] [--scale N] [WIDTH HEIGHT]
sketch-svg INPUT.sk OUTPUT.svg [--invert] [--scale N] [WIDTH HEIGHT]
sketch-gif OUTPUT.gif INPUT.sk... [--delay CENTISECONDS] [--invert] [WIDTH HEIGHT]
```

`TYPE` is one of `i8`, `u8`, `i16`, `u16`, `i32`, `u32`, `i64`, or `u64`.
Signed decoding uses two's complement; unsigned decoding rejects negative or
out-of-range values. `--hex` accepts a `0x` prefix when decoding, `--group`
formats binary as four-bit groups, and `--explain` reports the encoded value’s
interpretations. Sketch output defaults to an 80 × 40 canvas when no size is
supplied. The complete, implementation-backed stream specification is in
[`docs/SKETCH_FORMAT.md`](docs/SKETCH_FORMAT.md).

## Project layout

```text
app/                    Command-line boundaries
include/                Public headers: binary, list, sketch, toolkit
src/                    Reusable C23 implementations
test/                   Executable regression tests
examples/               Binary sketch fixtures and golden visual outputs
docs/                   Architecture and sketch-stream format notes
resources/original/     Preserved source material
.github/workflows/      Continuous-integration quality gate
```

## Quality bar

```bash
make build
make test
make check
make coverage
make format
make format-check
```

The build applies `-Wall -Wextra -Wpedantic -Werror` to library consumers and
targets. The regression suite covers signed and unsigned binary boundaries,
hexadecimal and grouped conversion, integer and generic-list
traversal/insertion/erasure/sentinel/ownership states, and sketch line, block,
malformed-input, trace, P2 output, scaling, inversion, and clipping behaviour.

`robustness-tests` adds deterministic pseudo-random binary round trips,
malformed-byte checks, and arbitrary sketch-stream decoding under the same
strict build and sanitizer configuration. `make coverage` emits GCC `gcov`
reports for the exercised core modules. `make format` and `make format-check`
use the repository’s `.clang-format` configuration.

`make check` runs every executable test under Valgrind. GitHub Actions repeats
the strict build and test suite on every push and pull request, runs Valgrind
against all three test executables, and performs a separate
AddressSanitizer/UndefinedBehaviorSanitizer build.

## Design notes

The toolkit favours small ownership-aware C modules and thin command-line
boundaries. [`docs/ARCHITECTURE.md`](docs/ARCHITECTURE.md) explains that split;
[`docs/SKETCH_FORMAT.md`](docs/SKETCH_FORMAT.md) documents the compact rendering
format.

---

The name **C23 Systems Toolkit** is intentional: it describes a growing group
of complete, reusable C programs without tying the repository to one data
structure, format, or historical course.
