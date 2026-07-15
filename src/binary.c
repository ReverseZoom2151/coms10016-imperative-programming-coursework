#include "binary.h"

#include <stdbool.h>
#include <string.h>

static bool valid_width(unsigned bits) { return bits > 0 && bits <= 64; }

static uint64_t width_mask(unsigned bits) {
  return bits == 64 ? UINT64_MAX : (UINT64_C(1) << bits) - 1;
}

static unsigned hex_digits(unsigned bits) { return (bits + 3) / 4; }

static int hex_value(char digit) {
  if (digit >= '0' && digit <= '9') return digit - '0';
  if (digit >= 'a' && digit <= 'f') return digit - 'a' + 10;
  if (digit >= 'A' && digit <= 'F') return digit - 'A' + 10;
  return -1;
}

binary_status binary_encode_unsigned(uint64_t value, unsigned bits, char *output, size_t output_size) {
  if (!valid_width(bits) || output == NULL || output_size < (size_t)bits + 1) return BINARY_INVALID_ARGUMENT;
  if ((value & ~width_mask(bits)) != 0) return BINARY_OUT_OF_RANGE;
  for (unsigned index = 0; index < bits; ++index) {
    unsigned shift = bits - index - 1;
    output[index] = (value & (UINT64_C(1) << shift)) == 0 ? '0' : '1';
  }
  output[bits] = '\0';
  return BINARY_OK;
}

binary_status binary_encode_signed(int64_t value, unsigned bits, char *output, size_t output_size) {
  if (!valid_width(bits)) return BINARY_INVALID_ARGUMENT;
  if (bits < 64 && (value < -(INT64_C(1) << (bits - 1)) || value >= (INT64_C(1) << (bits - 1)))) return BINARY_OUT_OF_RANGE;
  return binary_encode_unsigned((uint64_t)value & width_mask(bits), bits, output, output_size);
}

binary_status binary_decode_unsigned(const char *input, unsigned bits, uint64_t *value) {
  if (!valid_width(bits) || input == NULL || value == NULL || strlen(input) != bits) return BINARY_INVALID_ARGUMENT;
  uint64_t result = 0;
  for (unsigned index = 0; index < bits; ++index) {
    if (input[index] != '0' && input[index] != '1') return BINARY_INVALID_DIGIT;
    result = (result << 1) | (uint64_t)(input[index] - '0');
  }
  *value = result;
  return BINARY_OK;
}

binary_status binary_decode_signed(const char *input, unsigned bits, int64_t *value) {
  uint64_t encoded;
  binary_status status = binary_decode_unsigned(input, bits, &encoded);
  if (status != BINARY_OK || value == NULL) return status;
  if (bits == 64 || (encoded & (UINT64_C(1) << (bits - 1))) == 0) *value = (int64_t)encoded;
  else *value = (int64_t)(encoded | ~width_mask(bits));
  return BINARY_OK;
}

binary_status binary_encode_hex_unsigned(uint64_t value, unsigned bits, char *output, size_t output_size) {
  static const char digits[] = "0123456789ABCDEF";
  if (!valid_width(bits) || output == NULL || output_size < (size_t)hex_digits(bits) + 1) return BINARY_INVALID_ARGUMENT;
  if ((value & ~width_mask(bits)) != 0) return BINARY_OUT_OF_RANGE;
  unsigned count = hex_digits(bits);
  for (unsigned index = 0; index < count; ++index) {
    unsigned shift = (count - index - 1) * 4;
    output[index] = digits[(value >> shift) & UINT64_C(0xF)];
  }
  output[count] = '\0';
  return BINARY_OK;
}

binary_status binary_encode_hex_signed(int64_t value, unsigned bits, char *output, size_t output_size) {
  if (!valid_width(bits)) return BINARY_INVALID_ARGUMENT;
  if (bits < 64 && (value < -(INT64_C(1) << (bits - 1)) || value >= (INT64_C(1) << (bits - 1)))) return BINARY_OUT_OF_RANGE;
  return binary_encode_hex_unsigned((uint64_t)value & width_mask(bits), bits, output, output_size);
}

binary_status binary_decode_hex_unsigned(const char *input, unsigned bits, uint64_t *value) {
  if (!valid_width(bits) || input == NULL || value == NULL) return BINARY_INVALID_ARGUMENT;
  if (input[0] == '0' && (input[1] == 'x' || input[1] == 'X')) input += 2;
  unsigned count = hex_digits(bits);
  if (strlen(input) != count) return BINARY_INVALID_ARGUMENT;
  uint64_t result = 0;
  for (unsigned index = 0; index < count; ++index) {
    int digit = hex_value(input[index]);
    if (digit < 0) return BINARY_INVALID_DIGIT;
    result = (result << 4) | (uint64_t)digit;
  }
  if ((result & ~width_mask(bits)) != 0) return BINARY_OUT_OF_RANGE;
  *value = result;
  return BINARY_OK;
}

binary_status binary_decode_hex_signed(const char *input, unsigned bits, int64_t *value) {
  uint64_t encoded;
  binary_status status = binary_decode_hex_unsigned(input, bits, &encoded);
  if (status != BINARY_OK || value == NULL) return status;
  if (bits == 64 || (encoded & (UINT64_C(1) << (bits - 1))) == 0) *value = (int64_t)encoded;
  else *value = (int64_t)(encoded | ~width_mask(bits));
  return BINARY_OK;
}

binary_status binary_format_grouped(const char *input, unsigned group_size, char *output, size_t output_size) {
  if (input == NULL || output == NULL || group_size == 0) return BINARY_INVALID_ARGUMENT;
  size_t length = strlen(input);
  if (length == 0) return BINARY_INVALID_ARGUMENT;
  for (size_t index = 0; index < length; ++index)
    if (input[index] != '0' && input[index] != '1') return BINARY_INVALID_DIGIT;
  size_t separators = (length - 1) / group_size;
  if (output_size < length + separators + 1) return BINARY_INVALID_ARGUMENT;
  size_t first_group = length % group_size;
  if (first_group == 0) first_group = group_size;
  size_t input_index = 0;
  size_t output_index = 0;
  size_t group_length = first_group;
  while (input_index < length) {
    for (size_t index = 0; index < group_length; ++index) output[output_index++] = input[input_index++];
    if (input_index < length) output[output_index++] = '_';
    group_length = group_size;
  }
  output[output_index] = '\0';
  return BINARY_OK;
}

const char *binary_status_message(binary_status status) {
  switch (status) {
    case BINARY_OK: return "ok";
    case BINARY_INVALID_ARGUMENT: return "invalid argument";
    case BINARY_INVALID_DIGIT: return "binary text must contain only 0 or 1";
    case BINARY_OUT_OF_RANGE: return "value does not fit the requested width";
  }
  return "unknown error";
}
