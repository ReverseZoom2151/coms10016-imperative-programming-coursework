<div align="center">

# C23 Systems Toolkit

### A portable C23 command-line toolkit for exact data, safe containers, and terminal-scale rendering.

[![CI](https://github.com/ReverseZoom2151/c23-systems-toolkit/actions/workflows/ci.yml/badge.svg)](https://github.com/ReverseZoom2151/c23-systems-toolkit/actions/workflows/ci.yml)

</div>

This repository is a toolkit, not one monolithic program. It contains four
independent C components that share the same approach: explicit state, thin
command-line boundaries, strict validation, and reproducible output.

| Component | Input | What it demonstrates | Output |
| --- | --- | --- | --- |
| **Binary** | Decimal, binary, or hexadecimal values | Fixed widths, range validation, and two's-complement interpretation | Bits, grouped bits, hex, or an explanation |
| **Lists** | Insert, traverse, replace, erase, and release operations | Circular links, sentinel boundaries, cursor movement, and pointer ownership | A reusable C library and a runnable trace |
| **Sketches** | Compact `.sk` byte streams | Opcode decoding, rasterisation, clipping, grayscale pixels, and file output | Trace text, ASCII, P2/P5 PGM, SVG, and animated GIF |
| **Renderer** | Two rotation angles and a canvas | Surface sampling, projection, inverse-depth buffering, and luminance | Terminal frame, SVG, and animated GIF |

## Run it

You need a C23-capable compiler, [CMake](https://cmake.org/) 3.20+, and
optionally [Ninja](https://ninja-build.org/). The full local quality target
also uses [Valgrind](https://valgrind.org/) and `clang-format`.

```bash
git clone https://github.com/ReverseZoom2151/c23-systems-toolkit.git
cd c23-systems-toolkit
cmake -S . -B build -G Ninja
cmake --build build
ctest --test-dir build --output-on-failure
```

## 1. Exact binary representations

`binary-tool` never guesses a platform integer width. You name the type, and
it either represents the value exactly or reports an error. Signed values use
two's complement.

```bash
./build/binary-tool encode --explain i8 -12
```

```text
type: i8
width: 8 bits
signed: yes
binary: 1111_0100
hexadecimal: 0xF4
unsigned interpretation: 244
two's-complement interpretation: -12
```

This is the same eight stored bits viewed in three useful ways:

```text
decimal input        -12
stored bits       1 111 0100
grouped bits       1111_0100
hexadecimal              F4
```

The tool supports `i8`, `u8`, `i16`, `u16`, `i32`, `u32`, `i64`, and `u64`.
Use `--hex` to encode/decode hexadecimal, `--group` for four-bit grouping, and
`--explain` to print the interpretations shown above.

The Manim explainer turns that exact conversion into a high-resolution motion
story. The orange cell is the sign bit; the remaining cells are the stored
magnitude bits.

<p align="center">
  <img src="examples/binary-explainer.gif" alt="High-resolution animation of negative twelve becoming eight stored bits and hexadecimal F4" width="800" />
</p>

## 2. Circular lists with an observable cursor

There are two list libraries:

- `int_list` is the focused integer list used by `list-demo`.
- `generic_list` stores `void *` values and accepts an optional destructor, so
  ownership is explicit when values are replaced, erased, released, or the
  list is destroyed.

Both are circular doubly linked lists with a sentinel. The cursor is either on
an item or at the sentinel/end state; operations return `false` at invalid
boundaries rather than dereferencing an invalid pointer.

```bash
./build/list-demo
```

```text
insert after end (20):   [20]    cursor: 20
insert before (10):      [10] <-> [20]    cursor: 10
insert before (15):      [10] <-> [15] <-> [20]    cursor: 15
erase 20, move back:     [10] <-> [15]    cursor: 15
```

The same final state can be read structurally as:

```text
[end] <-> [10] <-> [15] <-> [end]
                     ^
                   cursor
```

The public APIs are in [`include/list.h`](include/list.h) and
[`include/generic_list.h`](include/generic_list.h).

The high-resolution explainer runs the same operations as `list-demo`; its
animation keeps the cursor attached to the node that each operation selects.

<p align="center">
  <img src="examples/list-explainer.gif" alt="High-resolution animation of linked-list nodes and cursor through four operations" width="800" />
</p>

## 3. Compact sketches: bytes to real visual output

A `.sk` file is a compact drawing program: every byte has a two-bit opcode and
a six-bit operand. The decoder tracks a cursor, target position, tool, colour,
and accumulated data; it can then inspect, rasterise, or export the same
stream without a GUI framework.

```bash
./build/sketch-inspect examples/gallery.sk
./build/toolkit-visualize sketch examples/gallery.sk \
  build/sketch-story.svg build/sketch-story.gif
```

The explainer first shows the actual `gallery.sk` byte stream. It then pairs
each C-generated canvas checkpoint with the matching `sketch-inspect` decoder
event: byte offset, opcode, cursor movement, and colour. The canvas is never
redrawn or invented by the animation.

<p align="center">
  <img src="examples/sketch-explainer.gif" alt="High-resolution animation of compact sketch drawing checkpoints" width="800" />
</p>

The committed [`examples/`](examples/) directory contains the source streams,
P2 PGM, ASCII output, instruction trace, GIF explainers, and the C fixture
generator used to reproduce them. The full format is documented in
[`docs/SKETCH_FORMAT.md`](docs/SKETCH_FORMAT.md).

## 4. A readable 3D terminal renderer

`donut-demo` turns the same grayscale canvas into a small 3D renderer. For
each frame it samples points on a torus, rotates and projects them, retains the
nearest sample at every pixel, then maps its lighting value to the terminal
ramp `.,-~:;=!*#$@`. The result is intentionally finite and reproducible: it
is a renderer that can animate, test, and export—not a one-line novelty.

```bash
./build/donut-demo --incremental 80 24
./build/donut-animate --incremental --frames 120 --fps 24
./build/donut-fixed --frames 24 --gif fixed.gif 80 24
./build/render-diagnostics raymarch raymarch.svg
./build/toolkit-visualize renderer build/renderer-story.svg build/renderer-story.gif
```

`donut-animate` remains intentionally terminal-sized, while `donut-fixed`
provides a Q30, no-`libm` comparison renderer with CORDIC or fast-approximate
trigonometry. `render-diagnostics` exposes depth, normals, ordered dithering,
and a bounded software ray-march torus. The README animation is a standalone
960 × 540 GIF assembled directly from sixteen real ANSI frames captured from
`donut-animate --incremental 80 24`; it has no explanatory wrapper and
preserves the renderer’s actual luminance characters and motion.

The optional [`animations/`](animations/) companion uses Manim Community for
the binary, list, and sketch explainers. The donut preview is generated without
Manim from the renderer's own terminal output. This tooling remains separate
from the portable C build and CI.

<p align="center">
  <img src="examples/renderer-explainer.gif" alt="High-resolution animated torus explaining sampling, depth testing, and lighting" width="800" />
</p>

[`docs/RENDERING.md`](docs/RENDERING.md) explains the geometry, depth buffer,
and lighting model; [`docs/ADVANCED_RENDERING.md`](docs/ADVANCED_RENDERING.md)
documents the fixed-point, diagnostic, and ray-march experiments.

## Command reference

```text
binary-tool encode|decode [--hex] [--group] [--explain] TYPE VALUE
list-demo
donut-demo [--incremental] [--frames N] [--gif OUTPUT.gif] [--svg OUTPUT.svg] [WIDTH HEIGHT]
donut-animate [--incremental] [--frames N] [--fps N] [WIDTH HEIGHT]
donut-fixed [--fast] [--frames N] [--gif OUTPUT.gif] [--svg OUTPUT.svg] [WIDTH HEIGHT]
render-diagnostics depth|normals|dither|raymarch OUTPUT.svg [WIDTH HEIGHT]
toolkit-visualize binary|list SVG_OUTPUT GIF_OUTPUT
toolkit-visualize sketch INPUT.sk SVG_OUTPUT GIF_OUTPUT
toolkit-visualize renderer SVG_OUTPUT GIF_OUTPUT
sketch-view INPUT.sk [WIDTH HEIGHT]
sketch-inspect INPUT.sk
sketch-pgm INPUT.sk OUTPUT.pgm [--plain] [--invert] [--scale N] [WIDTH HEIGHT]
sketch-svg INPUT.sk OUTPUT.svg [--invert] [--scale N] [WIDTH HEIGHT]
sketch-gif OUTPUT.gif INPUT.sk... [--delay CENTISECONDS] [--invert] [WIDTH HEIGHT]
```

`sketch-pgm --plain` writes inspectable P2 text; without it, P5 is compact
binary output. `sketch-svg` is useful for vector diagnostics and reports.
`sketch-gif` accepts same-sized full sketch streams as frames and loops them.

## Project layout

```text
app/                    Thin command-line boundaries
include/                Public binary, list, generic-list, sketch, donut, and toolkit APIs
src/                    Reusable C23 implementations
test/                   Executable regression and deterministic robustness tests
examples/               Reproducible sketch streams, images, traces, and generator
docs/                   Architecture, format, and rendering documentation
animations/             Optional Manim Community high-resolution explainers
.github/workflows/      CI: format, build, tests, Valgrind, and sanitizers
```

## Quality bar

```bash
make build        # configure and compile
make test         # run thirteen executable regression and visualization tests
make check        # tests plus Valgrind for every test executable
make coverage     # build with gcov instrumentation and emit reports
make format       # apply the repository's clang-format style
make format-check # verify formatting without changing files
```

The project compiles with `-Wall -Wextra -Wpedantic -Werror`. GitHub Actions
runs formatting, all tests, Valgrind, and a separate
AddressSanitizer/UndefinedBehaviorSanitizer build on every push and pull
request.

## Design notes

The components intentionally remain separate: binary conversion teaches data
representation; the lists teach pointer ownership and invariants; sketches
teach bytecode decoding and raster output. The shared design is small reusable
cores with thin terminal adapters, described in
[`docs/ARCHITECTURE.md`](docs/ARCHITECTURE.md).

---

The name **C23 Systems Toolkit** describes a growing collection of complete,
reusable C programs without tying the repository to one data structure or
format.
