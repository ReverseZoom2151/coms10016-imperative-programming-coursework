#include "sketch.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "usage: sketch-inspect FILE.sk\n");
        return EXIT_FAILURE;
    }
    sketch_status status = sketch_inspect_file(stdout, argv[1]);
    if (status != SKETCH_OK) {
        fprintf(stderr, "sketch-inspect: %s\n", sketch_status_message(status));
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
