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
const char *binary_status_message(binary_status status);

#endif
