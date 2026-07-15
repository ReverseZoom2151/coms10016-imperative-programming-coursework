#include "sketch.h"

#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum { OPERAND_MASK = 0x3f, OPERAND_SIGN = 0x20, OPERAND_BITS = 6 };

typedef enum { TOOL, DX, DY, DATA } opcode;
typedef enum {
    TOOL_NONE,
    TOOL_LINE,
    TOOL_BLOCK,
    TOOL_COLOUR,
    TOOL_TARGET_X,
    TOOL_TARGET_Y,
    TOOL_SHOW,
    TOOL_PAUSE,
    TOOL_NEXT_FRAME
} tool_command;
typedef enum { DRAW_NONE, DRAW_LINE, DRAW_BLOCK } drawing_tool;

struct sketch_canvas {
    size_t width;
    size_t height;
    uint8_t *pixels;
};

typedef struct {
    int x;
    int y;
    int target_x;
    int target_y;
    uint64_t data;
    uint8_t colour;
    drawing_tool tool;
} decoder_state;

static bool valid_canvas(const sketch_canvas *canvas) {
    return canvas != NULL && canvas->pixels != NULL;
}

static bool checked_add(int value, int change, int *result) {
    intmax_t total = (intmax_t)value + change;
    if (total < INT_MIN || total > INT_MAX) {
        return false;
    }
    *result = (int)total;
    return true;
}

static void set_pixel(sketch_canvas *canvas, int x, int y, uint8_t colour) {
    if (x < 0 || y < 0 || (size_t)x >= canvas->width ||
        (size_t)y >= canvas->height) {
        return;
    }
    canvas->pixels[(size_t)y * canvas->width + (size_t)x] = colour;
}

static unsigned point_outcode(int x, int y, int right, int bottom) {
    unsigned code = 0;
    if (x < 0) {
        code |= 1;
    } else if (x > right) {
        code |= 2;
    }
    if (y < 0) {
        code |= 4;
    } else if (y > bottom) {
        code |= 8;
    }
    return code;
}

static bool clip_line(const sketch_canvas *canvas, int *x0, int *y0, int *x1,
                      int *y1) {
    int right = canvas->width - 1 > INT_MAX ? INT_MAX : (int)(canvas->width - 1);
    int bottom = canvas->height - 1 > INT_MAX ? INT_MAX : (int)(canvas->height - 1);
    unsigned out0 = point_outcode(*x0, *y0, right, bottom);
    unsigned out1 = point_outcode(*x1, *y1, right, bottom);

    while ((out0 | out1) != 0) {
        if ((out0 & out1) != 0) {
            return false;
        }
        unsigned outside = out0 != 0 ? out0 : out1;
        int64_t x = 0;
        int64_t y = 0;
        if ((outside & 4) != 0) {
            if (*y1 == *y0) {
                return false;
            }
            x = (int64_t)*x0 + ((int64_t)*x1 - *x0) * -(*y0) / (*y1 - *y0);
            y = 0;
        } else if ((outside & 8) != 0) {
            if (*y1 == *y0) {
                return false;
            }
            x = (int64_t)*x0 + ((int64_t)*x1 - *x0) * (bottom - *y0) / (*y1 - *y0);
            y = bottom;
        } else if ((outside & 2) != 0) {
            if (*x1 == *x0) {
                return false;
            }
            x = right;
            y = (int64_t)*y0 + ((int64_t)*y1 - *y0) * (right - *x0) / (*x1 - *x0);
        } else {
            if (*x1 == *x0) {
                return false;
            }
            x = 0;
            y = (int64_t)*y0 + ((int64_t)*y1 - *y0) * -(*x0) / (*x1 - *x0);
        }
        if (outside == out0) {
            *x0 = (int)x;
            *y0 = (int)y;
            out0 = point_outcode(*x0, *y0, right, bottom);
        } else {
            *x1 = (int)x;
            *y1 = (int)y;
            out1 = point_outcode(*x1, *y1, right, bottom);
        }
    }
    return true;
}

static void draw_line(sketch_canvas *canvas, int x0, int y0, int x1, int y1,
                      uint8_t colour) {
    if (!clip_line(canvas, &x0, &y0, &x1, &y1)) {
        return;
    }
    int64_t dx = llabs((int64_t)x1 - x0);
    int step_x = x0 < x1 ? 1 : -1;
    int64_t dy = -llabs((int64_t)y1 - y0);
    int step_y = y0 < y1 ? 1 : -1;
    int64_t error = dx + dy;

    for (;;) {
        set_pixel(canvas, x0, y0, colour);
        if (x0 == x1 && y0 == y1) {
            return;
        }
        int64_t doubled_error = 2 * error;
        if (doubled_error >= dy) {
            error += dy;
            x0 += step_x;
        }
        if (doubled_error <= dx) {
            error += dx;
            y0 += step_y;
        }
    }
}

static void draw_block(sketch_canvas *canvas, int x0, int y0, int x1, int y1,
                       uint8_t colour) {
    int left = x0 < x1 ? x0 : x1;
    int right = x0 < x1 ? x1 : x0;
    int top = y0 < y1 ? y0 : y1;
    int bottom = y0 < y1 ? y1 : y0;
    int canvas_right = canvas->width - 1 > INT_MAX ? INT_MAX : (int)(canvas->width - 1);
    int canvas_bottom =
        canvas->height - 1 > INT_MAX ? INT_MAX : (int)(canvas->height - 1);
    if (right < 0 || bottom < 0 || left > canvas_right || top > canvas_bottom) {
        return;
    }
    left = left < 0 ? 0 : left;
    right = right > canvas_right ? canvas_right : right;
    top = top < 0 ? 0 : top;
    bottom = bottom > canvas_bottom ? canvas_bottom : bottom;
    for (int64_t y = top; y <= bottom; y++) {
        for (int64_t x = left; x <= right; x++) {
            set_pixel(canvas, (int)x, (int)y, colour);
        }
    }
}

static int signed_operand(uint8_t byte) {
    int value = byte & OPERAND_MASK;
    return (value & OPERAND_SIGN) == 0 ? value : value - (OPERAND_MASK + 1);
}

static sketch_status use_tool(decoder_state *state, uint64_t command) {
    switch (command) {
    case TOOL_NONE:
        state->tool = DRAW_NONE;
        break;
    case TOOL_LINE:
        state->tool = DRAW_LINE;
        break;
    case TOOL_BLOCK:
        state->tool = DRAW_BLOCK;
        break;
    case TOOL_COLOUR:
        if (state->data > UINT8_MAX) {
            return SKETCH_OUT_OF_RANGE;
        }
        state->colour = (uint8_t)state->data;
        break;
    case TOOL_TARGET_X:
        if (state->data > INT_MAX) {
            return SKETCH_OUT_OF_RANGE;
        }
        state->target_x = (int)state->data;
        break;
    case TOOL_TARGET_Y:
        if (state->data > INT_MAX) {
            return SKETCH_OUT_OF_RANGE;
        }
        state->target_y = (int)state->data;
        break;
    case TOOL_SHOW:
    case TOOL_PAUSE:
    case TOOL_NEXT_FRAME:
        break;
    default:
        return SKETCH_MALFORMED_STREAM;
    }
    state->data = 0;
    return SKETCH_OK;
}

static const char *tool_command_name(uint64_t command) {
    static const char *const names[] = {"NONE", "LINE", "BLOCK", "COLOUR",
                                        "TARGET_X", "TARGET_Y", "SHOW", "PAUSE",
                                        "NEXT_FRAME"};
    return command < sizeof(names) / sizeof(names[0]) ? names[command] : "UNKNOWN";
}

static const char *drawing_tool_name(drawing_tool tool) {
    switch (tool) {
    case DRAW_NONE:
        return "none";
    case DRAW_LINE:
        return "line";
    case DRAW_BLOCK:
        return "block";
    }
    return "unknown";
}

sketch_canvas *sketch_canvas_create(size_t width, size_t height) {
    if (width == 0 || height == 0 || width > SIZE_MAX / height) {
        return NULL;
    }
    sketch_canvas *canvas = malloc(sizeof(*canvas));
    if (canvas == NULL) {
        return NULL;
    }
    canvas->pixels = malloc(width * height);
    if (canvas->pixels == NULL) {
        free(canvas);
        return NULL;
    }
    canvas->width = width;
    canvas->height = height;
    memset(canvas->pixels, UINT8_MAX, width * height);
    return canvas;
}

void sketch_canvas_destroy(sketch_canvas *canvas) {
    if (canvas != NULL) {
        free(canvas->pixels);
        free(canvas);
    }
}

void sketch_canvas_clear(sketch_canvas *canvas, uint8_t value) {
    if (valid_canvas(canvas)) {
        memset(canvas->pixels, value, canvas->width * canvas->height);
    }
}

size_t sketch_canvas_width(const sketch_canvas *canvas) {
    return valid_canvas(canvas) ? canvas->width : 0;
}

size_t sketch_canvas_height(const sketch_canvas *canvas) {
    return valid_canvas(canvas) ? canvas->height : 0;
}

uint8_t sketch_canvas_pixel(const sketch_canvas *canvas, size_t x, size_t y) {
    if (!valid_canvas(canvas) || x >= canvas->width || y >= canvas->height) {
        return 0;
    }
    return canvas->pixels[y * canvas->width + x];
}

sketch_status sketch_decode_bytes(sketch_canvas *canvas, const uint8_t *bytes,
                                  size_t length) {
    if (!valid_canvas(canvas) || (bytes == NULL && length != 0)) {
        return SKETCH_INVALID_ARGUMENT;
    }
    decoder_state state = {0};
    state.colour = 0;
    state.tool = DRAW_LINE;

    for (size_t index = 0; index < length; index++) {
        uint8_t byte = bytes[index];
        switch ((opcode)(byte >> OPERAND_BITS)) {
        case TOOL: {
            sketch_status status = use_tool(&state, byte & OPERAND_MASK);
            if (status != SKETCH_OK) {
                return status;
            }
            break;
        }
        case DX:
            if (!checked_add(state.target_x, signed_operand(byte), &state.target_x)) {
                return SKETCH_OUT_OF_RANGE;
            }
            break;
        case DY:
            if (!checked_add(state.target_y, signed_operand(byte), &state.target_y)) {
                return SKETCH_OUT_OF_RANGE;
            }
            if (state.tool == DRAW_LINE) {
                draw_line(canvas, state.x, state.y, state.target_x, state.target_y,
                          state.colour);
            } else if (state.tool == DRAW_BLOCK) {
                draw_block(canvas, state.x, state.y, state.target_x, state.target_y,
                           state.colour);
            }
            state.x = state.target_x;
            state.y = state.target_y;
            break;
        case DATA:
            if (state.data > (UINT64_MAX >> OPERAND_BITS)) {
                return SKETCH_OUT_OF_RANGE;
            }
            state.data = (state.data << OPERAND_BITS) | (byte & OPERAND_MASK);
            break;
        }
    }
    return SKETCH_OK;
}

sketch_status sketch_decode_file(sketch_canvas *canvas, const char *path) {
    if (!valid_canvas(canvas) || path == NULL) {
        return SKETCH_INVALID_ARGUMENT;
    }
    FILE *input = fopen(path, "rb");
    if (input == NULL) {
        return SKETCH_IO_ERROR;
    }
    if (fseek(input, 0, SEEK_END) != 0) {
        fclose(input);
        return SKETCH_IO_ERROR;
    }
    long file_size = ftell(input);
    if (file_size < 0 || (uintmax_t)file_size > SIZE_MAX ||
        fseek(input, 0, SEEK_SET) != 0) {
        fclose(input);
        return SKETCH_IO_ERROR;
    }
    size_t length = (size_t)file_size;
    uint8_t *bytes = length == 0 ? NULL : malloc(length);
    if (length != 0 && bytes == NULL) {
        fclose(input);
        return SKETCH_ALLOCATION_FAILURE;
    }
    sketch_status status = SKETCH_OK;
    if (length != 0 && fread(bytes, 1, length, input) != length) {
        status = SKETCH_IO_ERROR;
    } else {
        status = sketch_decode_bytes(canvas, bytes, length);
    }
    free(bytes);
    fclose(input);
    return status;
}

sketch_status sketch_write_ascii(const sketch_canvas *canvas, FILE *output) {
    static const char shades[] = "@%#*+=-:. ";
    if (!valid_canvas(canvas) || output == NULL) {
        return SKETCH_INVALID_ARGUMENT;
    }
    for (size_t y = 0; y < canvas->height; y++) {
        for (size_t x = 0; x < canvas->width; x++) {
            uint8_t pixel = canvas->pixels[y * canvas->width + x];
            size_t shade = (size_t)pixel * (sizeof(shades) - 2) / UINT8_MAX;
            if (fputc(shades[shade], output) == EOF) {
                return SKETCH_IO_ERROR;
            }
        }
        if (fputc('\n', output) == EOF) {
            return SKETCH_IO_ERROR;
        }
    }
    return SKETCH_OK;
}

sketch_status sketch_write_pgm_options(const sketch_canvas *canvas,
                                       const char *path,
                                       sketch_pgm_format format, bool invert,
                                       size_t scale) {
    if (!valid_canvas(canvas) || path == NULL || scale == 0 ||
        (format != SKETCH_PGM_BINARY && format != SKETCH_PGM_PLAIN) ||
        canvas->width > SIZE_MAX / scale || canvas->height > SIZE_MAX / scale) {
        return SKETCH_INVALID_ARGUMENT;
    }
    FILE *output = fopen(path, format == SKETCH_PGM_BINARY ? "wb" : "w");
    if (output == NULL) {
        return SKETCH_IO_ERROR;
    }
    sketch_status status = SKETCH_OK;
    size_t output_width = canvas->width * scale;
    size_t output_height = canvas->height * scale;
    if (fprintf(output, "%s\n%zu %zu\n255\n",
                format == SKETCH_PGM_BINARY ? "P5" : "P2", output_width,
                output_height) < 0) {
        status = SKETCH_IO_ERROR;
    }
    for (size_t y = 0; status == SKETCH_OK && y < canvas->height; y++) {
        for (size_t repeat_y = 0; repeat_y < scale && status == SKETCH_OK;
             repeat_y++) {
            for (size_t x = 0; x < canvas->width && status == SKETCH_OK; x++) {
                uint8_t pixel = canvas->pixels[y * canvas->width + x];
                uint8_t value = invert ? (uint8_t)(UINT8_MAX - pixel) : pixel;
                for (size_t repeat_x = 0; repeat_x < scale; repeat_x++) {
                    if (format == SKETCH_PGM_BINARY) {
                        if (fputc(value, output) == EOF) {
                            status = SKETCH_IO_ERROR;
                            break;
                        }
                    } else if (fprintf(output, "%u%s", (unsigned)value,
                                       repeat_x + 1 == scale && x + 1 == canvas->width
                                           ? "\n"
                                           : " ") < 0) {
                        status = SKETCH_IO_ERROR;
                        break;
                    }
                }
            }
        }
    }
    if (fclose(output) != 0) {
        status = SKETCH_IO_ERROR;
    }
    return status;
}

sketch_status sketch_write_pgm(const sketch_canvas *canvas, const char *path) {
    return sketch_write_pgm_options(canvas, path, SKETCH_PGM_BINARY, false, 1);
}

static sketch_status inspect_stream(FILE *output, const uint8_t *bytes,
                                    size_t length) {
    decoder_state state = {0};
    size_t frame = 0;
    state.tool = DRAW_LINE;
    for (size_t index = 0; index < length; index++) {
        uint8_t byte = bytes[index];
        opcode operation = (opcode)(byte >> OPERAND_BITS);
        if (fprintf(output, "%04zu  0x%02X  ", index, (unsigned)byte) < 0) {
            return SKETCH_IO_ERROR;
        }
        switch (operation) {
        case TOOL: {
            uint64_t command = byte & OPERAND_MASK;
            uint64_t data = state.data;
            sketch_status status = use_tool(&state, command);
            if (fprintf(output, "TOOL %-10s data=%ju", tool_command_name(command),
                        (uintmax_t)data) < 0) {
                return SKETCH_IO_ERROR;
            }
            if (status != SKETCH_OK) {
                if (fprintf(output, " -> error: %s\n", sketch_status_message(status)) <
                    0) {
                    return SKETCH_IO_ERROR;
                }
                return status;
            }
            if (command == TOOL_NEXT_FRAME) {
                frame++;
                if (fprintf(output, " -> frame=%zu", frame) < 0) {
                    return SKETCH_IO_ERROR;
                }
            }
            break;
        }
        case DX:
            if (!checked_add(state.target_x, signed_operand(byte), &state.target_x)) {
                return SKETCH_OUT_OF_RANGE;
            }
            if (fprintf(output, "DX %+d -> target=(%d,%d)", signed_operand(byte),
                        state.target_x, state.target_y) < 0) {
                return SKETCH_IO_ERROR;
            }
            break;
        case DY: {
            if (!checked_add(state.target_y, signed_operand(byte), &state.target_y)) {
                return SKETCH_OUT_OF_RANGE;
            }
            int old_x = state.x;
            int old_y = state.y;
            state.x = state.target_x;
            state.y = state.target_y;
            if (fprintf(output,
                        "DY %+d -> %s (%d,%d) -> (%d,%d), colour=%u",
                        signed_operand(byte), drawing_tool_name(state.tool), old_x, old_y,
                        state.x, state.y, (unsigned)state.colour) < 0) {
                return SKETCH_IO_ERROR;
            }
            break;
        }
        case DATA:
            if (state.data > (UINT64_MAX >> OPERAND_BITS)) {
                return SKETCH_OUT_OF_RANGE;
            }
            state.data = (state.data << OPERAND_BITS) | (byte & OPERAND_MASK);
            if (fprintf(output, "DATA %-6u -> data=%ju", (unsigned)(byte & OPERAND_MASK),
                        (uintmax_t)state.data) < 0) {
                return SKETCH_IO_ERROR;
            }
            break;
        }
        if (fprintf(output, " | state cursor=(%d,%d) target=(%d,%d) tool=%s colour=%u "
                            "frame=%zu\n",
                    state.x, state.y, state.target_x, state.target_y,
                    drawing_tool_name(state.tool), (unsigned)state.colour, frame) < 0) {
            return SKETCH_IO_ERROR;
        }
    }
    return SKETCH_OK;
}

sketch_status sketch_inspect_bytes(FILE *output, const uint8_t *bytes,
                                   size_t length) {
    if (output == NULL || (bytes == NULL && length != 0)) {
        return SKETCH_INVALID_ARGUMENT;
    }
    return inspect_stream(output, bytes, length);
}

sketch_status sketch_inspect_file(FILE *output, const char *path) {
    if (output == NULL || path == NULL) {
        return SKETCH_INVALID_ARGUMENT;
    }
    FILE *input = fopen(path, "rb");
    if (input == NULL) {
        return SKETCH_IO_ERROR;
    }
    if (fseek(input, 0, SEEK_END) != 0) {
        fclose(input);
        return SKETCH_IO_ERROR;
    }
    long file_size = ftell(input);
    if (file_size < 0 || (uintmax_t)file_size > SIZE_MAX ||
        fseek(input, 0, SEEK_SET) != 0) {
        fclose(input);
        return SKETCH_IO_ERROR;
    }
    size_t length = (size_t)file_size;
    uint8_t *bytes = length == 0 ? NULL : malloc(length);
    if (length != 0 && bytes == NULL) {
        fclose(input);
        return SKETCH_ALLOCATION_FAILURE;
    }
    sketch_status status = SKETCH_OK;
    if (length != 0 && fread(bytes, 1, length, input) != length) {
        status = SKETCH_IO_ERROR;
    } else {
        status = inspect_stream(output, bytes, length);
    }
    free(bytes);
    fclose(input);
    return status;
}

const char *sketch_status_message(sketch_status status) {
    switch (status) {
    case SKETCH_OK:
        return "ok";
    case SKETCH_INVALID_ARGUMENT:
        return "invalid argument";
    case SKETCH_ALLOCATION_FAILURE:
        return "allocation failure";
    case SKETCH_IO_ERROR:
        return "input/output error";
    case SKETCH_MALFORMED_STREAM:
        return "malformed sketch stream";
    case SKETCH_OUT_OF_RANGE:
        return "value out of range";
    }
    return "unknown error";
}
