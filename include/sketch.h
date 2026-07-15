#ifndef IMPERATIVE_TOOLKIT_SKETCH_H
#define IMPERATIVE_TOOLKIT_SKETCH_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

typedef struct sketch_canvas sketch_canvas;

typedef enum {
    SKETCH_OK,
    SKETCH_INVALID_ARGUMENT,
    SKETCH_ALLOCATION_FAILURE,
    SKETCH_IO_ERROR,
    SKETCH_MALFORMED_STREAM,
    SKETCH_OUT_OF_RANGE
} sketch_status;

typedef enum { SKETCH_PGM_BINARY, SKETCH_PGM_PLAIN } sketch_pgm_format;

sketch_canvas *sketch_canvas_create(size_t width, size_t height);
void sketch_canvas_destroy(sketch_canvas *canvas);
void sketch_canvas_clear(sketch_canvas *canvas, uint8_t value);
size_t sketch_canvas_width(const sketch_canvas *canvas);
size_t sketch_canvas_height(const sketch_canvas *canvas);
uint8_t sketch_canvas_pixel(const sketch_canvas *canvas, size_t x, size_t y);
/* Set one grayscale pixel. Coordinates outside the canvas are ignored. */
void sketch_canvas_set_pixel(sketch_canvas *canvas, size_t x, size_t y, uint8_t value);

/* Decode the compact two-bit-opcode sketch format into canvas. */
sketch_status sketch_decode_bytes(sketch_canvas *canvas, const uint8_t *bytes,
                                  size_t length);
sketch_status sketch_decode_file(sketch_canvas *canvas, const char *path);

sketch_status sketch_write_ascii(const sketch_canvas *canvas, FILE *output);
sketch_status sketch_write_pgm(const sketch_canvas *canvas, const char *path);
/* Write P5 (binary) or P2 (plain text) PGM, with optional inversion and scaling. */
sketch_status sketch_write_pgm_options(const sketch_canvas *canvas, const char *path,
                                       sketch_pgm_format format, bool invert,
                                       size_t scale);
/* Write a dependency-free, pixel-faithful SVG preview. */
sketch_status sketch_write_svg(const sketch_canvas *canvas, const char *path,
                               bool invert, size_t scale);
/* Write same-sized canvases as a looping grayscale GIF animation. */
sketch_status sketch_write_gif(const sketch_canvas *const *frames, size_t frame_count,
                               const char *path, uint16_t delay_centiseconds,
                               bool invert);

/* Print a deterministic, human-readable execution trace without rasterising. */
sketch_status sketch_inspect_bytes(FILE *output, const uint8_t *bytes, size_t length);
sketch_status sketch_inspect_file(FILE *output, const char *path);
const char *sketch_status_message(sketch_status status);

#endif
