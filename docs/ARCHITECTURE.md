# Architecture

The maintained project is a portable C17 command-line toolkit. Reusable logic
lives in `src/` with public headers in `include/`; `app/` contains thin command
boundaries and `test/` contains executable regression tests.

The original coursework files live in `resources/original/`. They are retained
as source material only: several depend on unavailable display headers or do
not compile under strict C17. Their useful domains are being rebuilt as:

- binary conversion with explicit-width integer types;
- a circular doubly linked-list library;
- a self-contained sketch decoder with ASCII and PGM renderers.
