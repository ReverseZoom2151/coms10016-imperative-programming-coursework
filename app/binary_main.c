#include "binary.h"

#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { const char *name; unsigned bits; int is_signed; } numeric_type;

static const numeric_type types[] = {
  {"i8", 8, 1}, {"u8", 8, 0}, {"i16", 16, 1}, {"u16", 16, 0},
  {"i32", 32, 1}, {"u32", 32, 0}, {"i64", 64, 1}, {"u64", 64, 0}
};

static const numeric_type *find_type(const char *name) {
  for (size_t index = 0; index < sizeof(types) / sizeof(types[0]); ++index)
    if (strcmp(name, types[index].name) == 0) return &types[index];
  return NULL;
}

static uint64_t type_mask(const numeric_type *type) {
  return type->bits == 64 ? UINT64_MAX : (UINT64_C(1) << type->bits) - 1;
}

static void print_usage(FILE *stream) {
  fprintf(stream, "usage: binary-tool encode|decode [--hex] [--group] [--explain] TYPE VALUE\n");
  fprintf(stream, "TYPE: i8, u8, i16, u16, i32, u32, i64, or u64\n");
}

static int parse_decimal(const numeric_type *type, const char *text, uint64_t *encoded) {
  char *end = NULL;
  errno = 0;
  if (type->is_signed) {
    intmax_t value = strtoimax(text, &end, 10);
    if (errno != 0 || *text == '\0' || *end != '\0' || value < INT64_MIN || value > INT64_MAX) return 0;
    if (type->bits < 64 && (value < -(INT64_C(1) << (type->bits - 1)) || value >= (INT64_C(1) << (type->bits - 1)))) return 0;
    *encoded = (uint64_t)value & type_mask(type);
  } else {
    if (*text == '-') return 0;
    uintmax_t value = strtoumax(text, &end, 10);
    if (errno != 0 || *text == '\0' || *end != '\0' || value > UINT64_MAX || ((uint64_t)value & ~type_mask(type)) != 0) return 0;
    *encoded = (uint64_t)value;
  }
  return 1;
}

static int strip_group_separators(const char *input, char output[65]) {
  size_t output_index = 0;
  int previous_was_separator = 1;
  for (size_t index = 0; input[index] != '\0'; ++index) {
    if (input[index] == '_') {
      if (previous_was_separator) return 0;
      previous_was_separator = 1;
    } else {
      if (output_index == 64) return 0;
      output[output_index++] = input[index];
      previous_was_separator = 0;
    }
  }
  if (previous_was_separator) return 0;
  output[output_index] = '\0';
  return 1;
}

static void print_explanation(const numeric_type *type, uint64_t encoded) {
  char binary[65];
  char grouped[81];
  char hexadecimal[17];
  int64_t signed_value;
  (void)binary_encode_unsigned(encoded, type->bits, binary, sizeof(binary));
  (void)binary_format_grouped(binary, 4, grouped, sizeof(grouped));
  (void)binary_encode_hex_unsigned(encoded, type->bits, hexadecimal, sizeof(hexadecimal));
  printf("type: %s\nwidth: %u bits\nsigned: %s\nbinary: %s\nhexadecimal: 0x%s\nunsigned interpretation: %" PRIu64 "\n",
         type->name, type->bits, type->is_signed ? "yes" : "no", grouped, hexadecimal, encoded);
  if (type->is_signed) {
    (void)binary_decode_signed(binary, type->bits, &signed_value);
    printf("two's-complement interpretation: %" PRId64 "\n", signed_value);
  }
}

int main(int argc, char **argv) {
  if (argc < 4 || (strcmp(argv[1], "encode") != 0 && strcmp(argv[1], "decode") != 0)) {
    print_usage(stderr);
    return EXIT_FAILURE;
  }
  int hexadecimal = 0;
  int group = 0;
  int explain = 0;
  int argument = 2;
  while (argument < argc && strncmp(argv[argument], "--", 2) == 0) {
    if (strcmp(argv[argument], "--hex") == 0) hexadecimal = 1;
    else if (strcmp(argv[argument], "--group") == 0) group = 1;
    else if (strcmp(argv[argument], "--explain") == 0) explain = 1;
    else { print_usage(stderr); return EXIT_FAILURE; }
    ++argument;
  }
  if (argc - argument != 2 || (hexadecimal && group)) {
    if (hexadecimal && group) fprintf(stderr, "--group cannot be used with --hex\n");
    print_usage(stderr);
    return EXIT_FAILURE;
  }
  const numeric_type *type = find_type(argv[argument]);
  if (type == NULL) { fprintf(stderr, "TYPE must be i8, u8, i16, u16, i32, u32, i64, or u64\n"); return EXIT_FAILURE; }
  const char *text = argv[argument + 1];
  if (strcmp(argv[1], "encode") == 0) {
    uint64_t encoded;
    if (!parse_decimal(type, text, &encoded)) { fprintf(stderr, "value does not fit the requested type\n"); return EXIT_FAILURE; }
    if (explain) print_explanation(type, encoded);
    else if (hexadecimal) {
      char output[17];
      (void)binary_encode_hex_unsigned(encoded, type->bits, output, sizeof(output));
      puts(output);
    } else {
      char binary[65];
      char output[81];
      (void)binary_encode_unsigned(encoded, type->bits, binary, sizeof(binary));
      if (group) (void)binary_format_grouped(binary, 4, output, sizeof(output));
      else strcpy(output, binary);
      puts(output);
    }
  } else {
    char normalized[65];
    const char *input = text;
    if (!hexadecimal) {
      if (!strip_group_separators(text, normalized)) { fprintf(stderr, "invalid grouped binary value\n"); return EXIT_FAILURE; }
      input = normalized;
    }
    uint64_t encoded;
    binary_status status = hexadecimal ? binary_decode_hex_unsigned(input, type->bits, &encoded)
                                       : binary_decode_unsigned(input, type->bits, &encoded);
    if (status != BINARY_OK) { fprintf(stderr, "%s\n", binary_status_message(status)); return EXIT_FAILURE; }
    if (explain) print_explanation(type, encoded);
    else if (type->is_signed) {
      char binary[65];
      int64_t value;
      (void)binary_encode_unsigned(encoded, type->bits, binary, sizeof(binary));
      (void)binary_decode_signed(binary, type->bits, &value);
      printf("%" PRId64 "\n", value);
    } else printf("%" PRIu64 "\n", encoded);
  }
  return EXIT_SUCCESS;
}
