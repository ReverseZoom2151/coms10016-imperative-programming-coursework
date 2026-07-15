# Architecture

The maintained project is a portable C23 command-line toolkit. Reusable logic
lives in `src/` with public headers in `include/`; `app/` contains thin command
boundaries and `test/` contains executable regression tests. CMake owns the
build graph, while the Makefile provides short local workflows.

The original coursework files live in `resources/original/`. They are retained
as source material only: several depend on unavailable display headers or do
not compile under strict C17. Their useful domains are being rebuilt as:

- binary conversion with explicit-width integer types;
- integer and generic ownership-aware circular doubly linked-list libraries;
- a self-contained sketch decoder with inspection, ASCII, and P2/P5 PGM
  renderers.

Every public module is covered by an executable CTest target. Local `make
check` and GitHub Actions run those tests under Valgrind; CI also builds and
tests with AddressSanitizer and UndefinedBehaviorSanitizer enabled.

The command-line layer remains intentionally narrow: `binary-tool` translates
between decimal, binary, and hexadecimal representations; `sketch-inspect`
turns opaque bytes into an execution trace; and the sketch renderers choose an
ASCII or PGM representation of the same canvas. The committed `examples/`
fixtures make that output visible without introducing a GUI dependency.
