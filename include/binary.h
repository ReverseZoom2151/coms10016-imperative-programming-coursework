#ifndef IMPERATIVE_BINARY_H
#define IMPERATIVE_BINARY_H

#include <stddef.h>
#include <stdint.h>

typedef enum {
  BINARY_OK,
  BINARY_INVALID_ARGUMENT,
  BINARY_INVALID_DIGIT,
  BINARY_OUT_OF_RANGE
} binary_status;

binary_status binary_encode_unsigned(uint64_t value, unsigned bits, char *output, size_t output_size);
binary_status binary_encode_signed(int64_t value, unsigned bits, char *output, size_t output_size);
binary_status binary_decode_unsigned(const char *input, unsigned bits, uint64_t *value);
binary_status binary_decode_signed(const char *input, unsigned bits, int64_t *value);

/* Hexadecimal encodings contain ceil(bits / 4) uppercase digits and no prefix. */
binary_status binary_encode_hex_unsigned(uint64_t value, unsigned bits, char *output, size_t output_size);
binary_status binary_encode_hex_signed(int64_t value, unsigned bits, char *output, size_t output_size);
/* A leading 0x or 0X is accepted. Any unused high bits must be zero. */
binary_status binary_decode_hex_unsigned(const char *input, unsigned bits, uint64_t *value);
binary_status binary_decode_hex_signed(const char *input, unsigned bits, int64_t *value);

/* Inserts underscores between right-aligned groups; 11110100 -> 1111_0100. */
binary_status binary_format_grouped(const char *input, unsigned group_size, char *output, size_t output_size);
const char *binary_status_message(binary_status status);

#endif
