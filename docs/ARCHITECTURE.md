# Architecture

C23 Systems Toolkit is a portable command-line project organised around small,
reusable cores. Public APIs live in `include/`, implementations in `src/`,
thin executable boundaries in `app/`, and executable regression tests in
`test/`. CMake owns the build graph; the Makefile provides short local quality
commands.

## Components

- **Binary** provides exact fixed-width binary and hexadecimal conversion.
  `binary-tool` is the terminal boundary for its explicit-range API.
- **Lists** provide integer and generic ownership-aware circular doubly linked
  lists. `list-demo` makes cursor movement and mutation observable.
- **Sketches** decode compact byte streams into a grayscale `sketch_canvas`.
  The same decoded canvas can be inspected, printed, or exported as PGM, SVG,
  or GIF.
- **Renderer** samples and lights a 3D torus into that same grayscale canvas.
  The floating-point reference path, incremental recurrence, Q30/CORDIC
  comparison path, finite ANSI playback, and bounded diagnostics all share
  the canvas exporters without needing a graphics framework.
- **Visual stories** run real binary, list, sketch, and renderer operations,
  then emit labelled SVG diagnostics plus state-progression GIFs.
  `toolkit-visualize` is the deterministic C boundary; the README uses the
  matching high-resolution animation pipeline from `animations/`.

The shared canvas is the meaningful connection between the visual components:
sketches create pixels from a compact instruction stream; the renderer creates
pixels from sampled geometry. They share output formats, but neither module
needs to know the other's input format or scene logic.

## Quality boundary

Public functions return explicit status values or booleans. Invalid input,
integer width, ownership transfer, allocation, malformed byte streams, and
file I/O are all represented as observable outcomes rather than implicit
behaviour. Every component has an executable CTest target.

Local `make check` and GitHub Actions run tests under Valgrind. CI also checks
formatting and performs a separate AddressSanitizer/UndefinedBehaviorSanitizer
build. Committed examples and README explainers are reproducible outputs from
named C or Manim generators, rather than design mockups.
