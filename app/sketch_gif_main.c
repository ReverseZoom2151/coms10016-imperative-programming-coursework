#include "sketch.h"

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

int main(int argc, char *argv[]) {
    size_t width = 80;
    size_t height = 40;
    size_t delay = 40;
    size_t dimensions[2] = {0};
    size_t dimension_count = 0;
    int invert = 0;
    if (argc < 3) {
        fprintf(stderr,
                "usage: sketch-gif OUTPUT.gif INPUT.sk... [--delay CENTISECONDS] "
                "[--invert] [WIDTH HEIGHT]\n");
        return EXIT_FAILURE;
    }
    const char **inputs = calloc((size_t)argc - 2, sizeof(*inputs));
    if (inputs == NULL) {
        fprintf(stderr, "could not allocate input list\n");
        return EXIT_FAILURE;
    }
    size_t input_count = 0;

    for (int index = 2; index < argc; index++) {
        if (strcmp(argv[index], "--invert") == 0) {
            invert = 1;
        } else if (strcmp(argv[index], "--delay") == 0) {
            if (++index == argc || !parse_size(argv[index], &delay) ||
                delay > UINT16_MAX) {
                fprintf(stderr, "--delay must be an integer from 1 to 65535\n");
                free(inputs);
                return EXIT_FAILURE;
            }
        } else if (dimension_count < 2 &&
                   parse_size(argv[index], &dimensions[dimension_count])) {
            dimension_count++;
        } else {
            inputs[input_count++] = argv[index];
        }
    }
    if (input_count == 0 || (dimension_count != 0 && dimension_count != 2)) {
        fprintf(stderr,
                "usage: sketch-gif OUTPUT.gif INPUT.sk... [--delay CENTISECONDS] "
                "[--invert] [WIDTH HEIGHT]\n");
        free(inputs);
        return EXIT_FAILURE;
    }
    if (dimension_count == 2) {
        width = dimensions[0];
        height = dimensions[1];
    }

    sketch_canvas **frames = calloc(input_count, sizeof(*frames));
    if (frames == NULL) {
        fprintf(stderr, "could not allocate canvas list\n");
        free(inputs);
        return EXIT_FAILURE;
    }
    sketch_status status = SKETCH_OK;
    for (size_t index = 0; index < input_count && status == SKETCH_OK; index++) {
        frames[index] = sketch_canvas_create(width, height);
        if (frames[index] == NULL) {
            status = SKETCH_ALLOCATION_FAILURE;
        } else {
            status = sketch_decode_file(frames[index], inputs[index]);
        }
    }
    if (status == SKETCH_OK) {
        status = sketch_write_gif((const sketch_canvas *const *)frames, input_count,
                                  argv[1], (uint16_t)delay, invert != 0);
    }
    for (size_t index = 0; index < input_count; index++) {
        sketch_canvas_destroy(frames[index]);
    }
    free(frames);
    free(inputs);
    if (status != SKETCH_OK) {
        fprintf(stderr, "sketch-gif: %s\n", sketch_status_message(status));
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
