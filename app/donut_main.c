#include "donut.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int parse_size(const char *text, size_t *value) {
    char *end = NULL;
    errno = 0;
    unsigned long long parsed = strtoull(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0' || parsed == 0 || parsed > SIZE_MAX) {
        return 0;
    }
    *value = (size_t)parsed;
    return 1;
}

static void print_usage(FILE *output) {
    fputs("usage: donut-demo [--frames N] [--gif OUTPUT.gif] [--svg OUTPUT.svg] "
          "[WIDTH HEIGHT]\n",
          output);
}

int main(int argc, char *argv[]) {
    size_t width = 80;
    size_t height = 24;
    size_t frame_count = 1;
    const char *gif_path = NULL;
    const char *svg_path = NULL;
    size_t dimensions[2] = {0};
    size_t dimension_count = 0;
    for (int index = 1; index < argc; index++) {
        if (strcmp(argv[index], "--frames") == 0) {
            if (++index == argc || !parse_size(argv[index], &frame_count)) {
                print_usage(stderr);
                return EXIT_FAILURE;
            }
        } else if (strcmp(argv[index], "--gif") == 0) {
            if (++index == argc) {
                print_usage(stderr);
                return EXIT_FAILURE;
            }
            gif_path = argv[index];
        } else if (strcmp(argv[index], "--svg") == 0) {
            if (++index == argc) {
                print_usage(stderr);
                return EXIT_FAILURE;
            }
            svg_path = argv[index];
        } else if (dimension_count < 2 &&
                   parse_size(argv[index], &dimensions[dimension_count])) {
            dimension_count++;
        } else {
            print_usage(stderr);
            return EXIT_FAILURE;
        }
    }
    if (dimension_count != 0 && dimension_count != 2) {
        print_usage(stderr);
        return EXIT_FAILURE;
    }
    if (dimension_count == 2) {
        width = dimensions[0];
        height = dimensions[1];
    }
    if (width > SIZE_MAX / height) {
        fputs("donut-demo: canvas is too large\n", stderr);
        return EXIT_FAILURE;
    }

    if (frame_count > SIZE_MAX / sizeof(sketch_canvas *)) {
        fputs("donut-demo: too many frames\n", stderr);
        return EXIT_FAILURE;
    }
    sketch_canvas **frames = calloc(frame_count, sizeof(*frames));
    if (frames == NULL) {
        fputs("donut-demo: allocation failure\n", stderr);
        return EXIT_FAILURE;
    }
    donut_status status = DONUT_OK;
    for (size_t index = 0; index < frame_count && status == DONUT_OK; index++) {
        frames[index] = sketch_canvas_create(width, height);
        if (frames[index] == NULL) {
            status = DONUT_ALLOCATION_FAILURE;
        } else {
            status = donut_render_frame(frames[index], (float)index * 0.10F,
                                        (float)index * 0.05F);
        }
    }
    if (status == DONUT_OK && gif_path != NULL) {
        status = sketch_write_gif((const sketch_canvas *const *)frames, frame_count,
                                  gif_path, 5, true) == SKETCH_OK
                     ? DONUT_OK
                     : DONUT_IO_ERROR;
    }
    if (status == DONUT_OK && svg_path != NULL) {
        status =
            sketch_write_svg(frames[frame_count - 1], svg_path, true, 8) == SKETCH_OK
                ? DONUT_OK
                : DONUT_IO_ERROR;
    }
    if (status == DONUT_OK && gif_path == NULL && svg_path == NULL) {
        status = donut_write_ascii(frames[frame_count - 1], stdout);
    }
    for (size_t index = 0; index < frame_count; index++) {
        sketch_canvas_destroy(frames[index]);
    }
    free(frames);
    if (status != DONUT_OK) {
        fprintf(stderr, "donut-demo: %s\n", donut_status_message(status));
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
