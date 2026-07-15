#include "binary.h"

#include <assert.h>
#include <string.h>

int main(void) {
  char output[81]; uint64_t unsigned_value; int64_t signed_value;
  assert(binary_encode_unsigned(255, 8, output, sizeof(output)) == BINARY_OK);
  assert(strcmp(output, "11111111") == 0);
  assert(binary_encode_signed(-12, 8, output, sizeof(output)) == BINARY_OK);
  assert(strcmp(output, "11110100") == 0);
  assert(binary_decode_unsigned("11111111", 8, &unsigned_value) == BINARY_OK && unsigned_value == 255);
  assert(binary_decode_signed("11110100", 8, &signed_value) == BINARY_OK && signed_value == -12);
  assert(binary_encode_hex_unsigned(255, 8, output, sizeof(output)) == BINARY_OK);
  assert(strcmp(output, "FF") == 0);
  assert(binary_encode_hex_signed(-12, 8, output, sizeof(output)) == BINARY_OK);
  assert(strcmp(output, "F4") == 0);
  assert(binary_encode_hex_unsigned(UINT64_MAX, 64, output, sizeof(output)) == BINARY_OK);
  assert(strcmp(output, "FFFFFFFFFFFFFFFF") == 0);
  assert(binary_decode_hex_unsigned("0xFf", 8, &unsigned_value) == BINARY_OK && unsigned_value == 255);
  assert(binary_decode_hex_signed("F4", 8, &signed_value) == BINARY_OK && signed_value == -12);
  assert(binary_decode_hex_unsigned("8", 3, &unsigned_value) == BINARY_OUT_OF_RANGE);
  assert(binary_decode_hex_unsigned("FG", 8, &unsigned_value) == BINARY_INVALID_DIGIT);
  assert(binary_format_grouped("11110100", 4, output, sizeof(output)) == BINARY_OK);
  assert(strcmp(output, "1111_0100") == 0);
  assert(binary_format_grouped("1010101", 3, output, sizeof(output)) == BINARY_OK);
  assert(strcmp(output, "1_010_101") == 0);
  assert(binary_format_grouped("1012", 4, output, sizeof(output)) == BINARY_INVALID_DIGIT);
  assert(binary_format_grouped("1", 0, output, sizeof(output)) == BINARY_INVALID_ARGUMENT);
  assert(binary_encode_unsigned(256, 8, output, sizeof(output)) == BINARY_OUT_OF_RANGE);
  assert(binary_decode_unsigned("0102", 4, &unsigned_value) == BINARY_INVALID_DIGIT);
  return 0;
}
