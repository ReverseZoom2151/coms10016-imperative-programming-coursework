#include "binary.h"
#include "sketch.h"

#include <assert.h>
#include <stdint.h>
#include <string.h>

static uint64_t next_random(uint64_t *state) {
    *state ^= *state << 13;
    *state ^= *state >> 7;
    *state ^= *state << 17;
    return *state;
}

static void test_binary_round_trips(void) {
    uint64_t state = UINT64_C(0x9e3779b97f4a7c15);
    for (unsigned bits = 1; bits <= 64; bits++) {
        for (unsigned iteration = 0; iteration < 128; iteration++) {
            uint64_t value = next_random(&state);
            uint64_t mask = bits == 64 ? UINT64_MAX : (UINT64_C(1) << bits) - 1;
            uint64_t expected = value & mask;
            char encoded[65];
            uint64_t decoded = 0;
            assert(binary_encode_unsigned(expected, bits, encoded, sizeof(encoded)) ==
                   BINARY_OK);
            assert(binary_decode_unsigned(encoded, bits, &decoded) == BINARY_OK);
            assert(decoded == expected);
        }
    }
}

static void test_malformed_binary_input(void) {
    char input[9] = "00000000";
    uint64_t decoded = 0;
    for (unsigned byte = 0; byte <= UINT8_MAX; byte++) {
        input[3] = (char)byte;
        binary_status status = binary_decode_unsigned(input, 8, &decoded);
        if (byte == '0' || byte == '1') {
            assert(status == BINARY_OK);
        } else if (byte == '\0') {
            assert(status == BINARY_INVALID_ARGUMENT);
        } else {
            assert(status == BINARY_INVALID_DIGIT);
        }
    }
}

static void test_arbitrary_sketch_streams(void) {
    uint64_t state = UINT64_C(0xd1b54a32d192ed03);
    sketch_canvas *canvas = sketch_canvas_create(32, 24);
    assert(canvas != NULL);
    for (unsigned case_number = 0; case_number < 256; case_number++) {
        uint8_t bytes[32];
        for (size_t index = 0; index < sizeof(bytes); index++) {
            bytes[index] = (uint8_t)next_random(&state);
        }
        sketch_canvas_clear(canvas, UINT8_MAX);
        sketch_status status = sketch_decode_bytes(canvas, bytes, sizeof(bytes));
        assert(status == SKETCH_OK || status == SKETCH_MALFORMED_STREAM ||
               status == SKETCH_OUT_OF_RANGE);
    }
    sketch_canvas_destroy(canvas);
}

int main(void) {
    test_binary_round_trips();
    test_malformed_binary_input();
    test_arbitrary_sketch_streams();
    return 0;
}
