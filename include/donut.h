#ifndef IMPERATIVE_TOOLKIT_DONUT_H
#define IMPERATIVE_TOOLKIT_DONUT_H

#include "sketch.h"

#include <stdio.h>

typedef enum {
    DONUT_OK,
    DONUT_INVALID_ARGUMENT,
    DONUT_ALLOCATION_FAILURE,
    DONUT_IO_ERROR
} donut_status;

/*
 * Rasterise one lit torus frame into a grayscale canvas.
 *
 * The angles are in radians. The renderer samples the torus surface, projects
 * it into the canvas, and keeps the nearest sample at each pixel using an
 * inverse-depth buffer. Dark pixels represent brighter surface points.
 */
donut_status donut_render_frame(sketch_canvas *canvas, float angle_a, float angle_b);

/* Write a terminal frame using the luminance ramp ".,-~:;=!*#$@". */
donut_status donut_write_ascii(const sketch_canvas *canvas, FILE *output);
const char *donut_status_message(donut_status status);

#endif
