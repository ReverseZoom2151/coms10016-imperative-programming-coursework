/* Regenerate the binary streams and their checked-in visual outputs. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static void write_stream(const char *path, const uint8_t *bytes, size_t length) {
    FILE *output = fopen(path, "wb");
    if (output == NULL) {
        perror(path);
        exit(EXIT_FAILURE);
    }
    int failed = fwrite(bytes, 1, length, output) != length;
    if (fclose(output) != 0) {
        failed = 1;
    }
    if (failed) {
        perror(path);
        exit(EXIT_FAILURE);
    }
}

static void write_byte(FILE *output, uint8_t byte) {
    if (fputc(byte, output) == EOF) {
        perror("writing sketch byte");
        exit(EXIT_FAILURE);
    }
}

static void write_data(FILE *output, unsigned value) {
    if (value >= 64) {
        write_byte(output, (uint8_t)(0xc0 | (value >> 6)));
    }
    write_byte(output, (uint8_t)(0xc0 | (value & 0x3f)));
}

static void write_tool(FILE *output, uint8_t tool) {
    write_byte(output, tool);
}

static void set_target_x(FILE *output, unsigned value) {
    write_data(output, value);
    write_tool(output, 0x04);
}

static void set_target_y(FILE *output, unsigned value) {
    write_data(output, value);
    write_tool(output, 0x05);
}

static void move_to(FILE *output, unsigned x, unsigned y) {
    write_tool(output, 0x00);
    set_target_x(output, x);
    set_target_y(output, y);
    write_byte(output, 0x80);
}

static void set_colour(FILE *output, unsigned value) {
    write_data(output, value);
    write_tool(output, 0x03);
}

static void draw_block(FILE *output, unsigned x0, unsigned y0, unsigned x1, unsigned y1,
                       unsigned colour) {
    move_to(output, x0, y0);
    write_tool(output, 0x02);
    set_colour(output, colour);
    set_target_x(output, x1);
    set_target_y(output, y1);
    write_byte(output, 0x80);
}

static void draw_line(FILE *output, unsigned x0, unsigned y0, unsigned x1, unsigned y1,
                      unsigned colour) {
    move_to(output, x0, y0);
    write_tool(output, 0x01);
    set_colour(output, colour);
    set_target_x(output, x1);
    set_target_y(output, y1);
    write_byte(output, 0x80);
}

static void write_gallery(const char *path, unsigned stage) {
    FILE *output = fopen(path, "wb");
    if (output == NULL) {
        perror(path);
        exit(EXIT_FAILURE);
    }

    draw_block(output, 0, 0, 31, 19, 238);
    draw_block(output, 2, 2, 29, 17, 210);
    if (stage >= 2) {
        draw_block(output, 6, 5, 9, 14, 255);
        draw_block(output, 9, 5, 20, 7, 255);
        draw_block(output, 9, 12, 20, 14, 255);
        draw_block(output, 9, 8, 13, 11, 210);
    }
    if (stage >= 3) {
        draw_line(output, 22, 5, 26, 9, 0);
        draw_line(output, 26, 9, 22, 13, 0);
    }

    if (fclose(output) != 0) {
        perror(path);
        exit(EXIT_FAILURE);
    }
}

int main(void) {
    const uint8_t line[] = {0xc0, 0x03, 0x83, 0x42, 0x80};
    const uint8_t blocks[] = {0x00, 0x42, 0x82, 0x02, 0x42, 0x81};
    const uint8_t clipping[] = {0xc8, 0x04, 0xc1, 0x05, 0x80};
    write_stream("line.sk", line, sizeof(line));
    write_stream("blocks.sk", blocks, sizeof(blocks));
    write_stream("clipping.sk", clipping, sizeof(clipping));
    write_gallery("gallery-frame-1.sk", 1);
    write_gallery("gallery-frame-2.sk", 2);
    write_gallery("gallery.sk", 3);
    return EXIT_SUCCESS;
}
