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

int main(void) {
    const uint8_t line[] = {0xc0, 0x03, 0x83, 0x42, 0x80};
    const uint8_t blocks[] = {0x00, 0x42, 0x82, 0x02, 0x42, 0x81};
    const uint8_t clipping[] = {0xc8, 0x04, 0xc1, 0x05, 0x80};
    write_stream("line.sk", line, sizeof(line));
    write_stream("blocks.sk", blocks, sizeof(blocks));
    write_stream("clipping.sk", clipping, sizeof(clipping));
    return EXIT_SUCCESS;
}
