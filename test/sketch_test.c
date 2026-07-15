#include "sketch.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    /* Set black, move down, then draw a horizontal line. */
    const uint8_t line[] = {0xc0, 0x03, 0x83, 0x42, 0x80};
    sketch_canvas *canvas = sketch_canvas_create(5, 5);
    assert(canvas != NULL);
    assert(sketch_decode_bytes(canvas, line, sizeof(line)) == SKETCH_OK);
    assert(sketch_canvas_pixel(canvas, 0, 3) == 0);
    assert(sketch_canvas_pixel(canvas, 1, 3) == 0);
    assert(sketch_canvas_pixel(canvas, 2, 3) == 0);
    assert(sketch_canvas_pixel(canvas, 3, 3) == UINT8_MAX);

    /* Move without drawing, then fill the rectangle from (2, 3) to (4, 4). */
    const uint8_t block[] = {0x00, 0xc2, 0x04, 0xc3, 0x05, 0x80,
                             0x02, 0xc4, 0x04, 0xc4, 0x05, 0x80};
    assert(sketch_decode_bytes(canvas, block, sizeof(block)) == SKETCH_OK);
    assert(sketch_canvas_pixel(canvas, 2, 3) == 0);
    assert(sketch_canvas_pixel(canvas, 4, 4) == 0);
    assert(sketch_canvas_pixel(canvas, 1, 4) == UINT8_MAX);

    sketch_canvas_clear(canvas, UINT8_MAX);
    /* An off-canvas endpoint is clipped instead of making rasterisation unbounded. */
    const uint8_t clipped[] = {0xff, 0x04, 0x80};
    assert(sketch_decode_bytes(canvas, clipped, sizeof(clipped)) == SKETCH_OK);
    for (size_t x = 0; x < 5; x++) {
        assert(sketch_canvas_pixel(canvas, x, 0) == 0);
    }

    assert(sketch_decode_bytes(canvas, (const uint8_t[]){0x3f}, 1) ==
           SKETCH_MALFORMED_STREAM);

    FILE *trace = tmpfile();
    assert(trace != NULL);
    const uint8_t inspected[] = {0xc0, 0x03, 0x41, 0x81, 0x08};
    assert(sketch_inspect_bytes(trace, inspected, sizeof(inspected)) == SKETCH_OK);
    assert(fseek(trace, 0, SEEK_SET) == 0);
    char trace_text[512] = {0};
    assert(fread(trace_text, 1, sizeof(trace_text) - 1, trace) > 0);
    assert(strstr(trace_text, "TOOL COLOUR") != NULL);
    assert(strstr(trace_text, "line (0,0) -> (1,1), colour=0") != NULL);
    assert(strstr(trace_text, "NEXT_FRAME") != NULL);
    assert(fclose(trace) == 0);

    assert(sketch_write_pgm_options(canvas, "sketch-test.pgm", SKETCH_PGM_PLAIN,
                                    true, 2) == SKETCH_OK);
    FILE *pgm = fopen("sketch-test.pgm", "rb");
    assert(pgm != NULL);
    char pgm_header[32] = {0};
    assert(fread(pgm_header, 1, sizeof(pgm_header) - 1, pgm) > 0);
    assert(strncmp(pgm_header, "P2\n10 10\n255\n", 13) == 0);
    assert(fclose(pgm) == 0);
    assert(remove("sketch-test.pgm") == 0);
    sketch_canvas_destroy(canvas);
    return 0;
}
