#include "donut.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static size_t nonwhite_pixels(const sketch_canvas *canvas) {
    size_t total = 0;
    for (size_t y = 0; y < sketch_canvas_height(canvas); y++) {
        for (size_t x = 0; x < sketch_canvas_width(canvas); x++) {
            total += sketch_canvas_pixel(canvas, x, y) != UINT8_MAX;
        }
    }
    return total;
}

int main(void) {
    assert(donut_render_frame(NULL, 0.0F, 0.0F) == DONUT_INVALID_ARGUMENT);
    assert(donut_write_ascii(NULL, stdout) == DONUT_INVALID_ARGUMENT);

    sketch_canvas *first = sketch_canvas_create(80, 24);
    sketch_canvas *second = sketch_canvas_create(80, 24);
    assert(first != NULL && second != NULL);
    assert(donut_render_frame(first, 0.0F, 0.0F) == DONUT_OK);
    assert(donut_render_frame(second, 0.8F, 0.4F) == DONUT_OK);
    assert(nonwhite_pixels(first) > 300);
    assert(nonwhite_pixels(second) > 300);

    size_t changed = 0;
    for (size_t y = 0; y < sketch_canvas_height(first); y++) {
        for (size_t x = 0; x < sketch_canvas_width(first); x++) {
            changed +=
                sketch_canvas_pixel(first, x, y) != sketch_canvas_pixel(second, x, y);
        }
    }
    assert(changed > 100);

    FILE *ascii = tmpfile();
    assert(ascii != NULL);
    assert(donut_write_ascii(first, ascii) == DONUT_OK);
    assert(fseek(ascii, 0, SEEK_SET) == 0);
    char output[4096] = {0};
    assert(fread(output, 1, sizeof(output) - 1, ascii) > 0);
    assert(strchr(output, '@') != NULL);
    assert(strchr(output, '\n') != NULL);
    assert(fclose(ascii) == 0);

    sketch_canvas_destroy(second);
    sketch_canvas_destroy(first);
    return 0;
}
