#include "sketch.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int parse_size(const char *text, size_t *value) {
    char *end = NULL;
    errno = 0;
    unsigned long long parsed = strtoull(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0' || parsed == 0 ||
        parsed > SIZE_MAX) {
        return 0;
    }
    *value = (size_t)parsed;
    return 1;
}

int main(int argc, char *argv[]) {
    size_t width = 80;
    size_t height = 40;
    size_t scale = 1;
    size_t dimensions[2] = {0};
    size_t dimension_count = 0;
    int invert = 0;
    sketch_pgm_format format = SKETCH_PGM_BINARY;
    if (argc < 3) {
        fprintf(stderr,
                "usage: sketch-pgm INPUT.sk OUTPUT.pgm [--plain] [--invert] "
                "[--scale N] [WIDTH HEIGHT]\n");
        return EXIT_FAILURE;
    }
    for (int index = 3; index < argc; index++) {
        if (strcmp(argv[index], "--plain") == 0) {
            format = SKETCH_PGM_PLAIN;
        } else if (strcmp(argv[index], "--invert") == 0) {
            invert = 1;
        } else if (strcmp(argv[index], "--scale") == 0) {
            if (++index == argc || !parse_size(argv[index], &scale)) {
                fprintf(stderr, "--scale requires a positive integer\n");
                return EXIT_FAILURE;
            }
        } else if (dimension_count < 2 && parse_size(argv[index], &dimensions[dimension_count])) {
            dimension_count++;
        } else {
            fprintf(stderr, "unknown option or invalid canvas size: %s\n", argv[index]);
            return EXIT_FAILURE;
        }
    }
    if (dimension_count != 0 && dimension_count != 2) {
        fprintf(stderr, "WIDTH and HEIGHT must be given together\n");
        return EXIT_FAILURE;
    }
    if (dimension_count == 2) {
        width = dimensions[0];
        height = dimensions[1];
    }
    sketch_canvas *canvas = sketch_canvas_create(width, height);
    if (canvas == NULL) {
        fprintf(stderr, "could not allocate canvas\n");
        return EXIT_FAILURE;
    }
    sketch_status status = sketch_decode_file(canvas, argv[1]);
    if (status == SKETCH_OK) {
        status = sketch_write_pgm_options(canvas, argv[2], format, invert != 0, scale);
    }
    sketch_canvas_destroy(canvas);
    if (status != SKETCH_OK) {
        fprintf(stderr, "sketch-pgm: %s\n", sketch_status_message(status));
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
