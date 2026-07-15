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

int main(int argc, char **argv) {
  if (argc != 4 || (strcmp(argv[1], "encode") != 0 && strcmp(argv[1], "decode") != 0)) {
    fprintf(stderr, "usage: binary-tool encode|decode TYPE VALUE\n");
    return EXIT_FAILURE;
  }
  const numeric_type *type = find_type(argv[2]);
  if (type == NULL) { fprintf(stderr, "TYPE must be i8, u8, i16, u16, i32, u32, i64, or u64\n"); return EXIT_FAILURE; }
  if (strcmp(argv[1], "encode") == 0) {
    char output[65]; errno = 0; char *end = NULL;
    binary_status status;
    if (type->is_signed) {
      intmax_t value = strtoimax(argv[3], &end, 10);
      status = errno || *end ? BINARY_INVALID_ARGUMENT : binary_encode_signed((int64_t)value, type->bits, output, sizeof(output));
    } else {
      uintmax_t value = strtoumax(argv[3], &end, 10);
      status = errno || *end ? BINARY_INVALID_ARGUMENT : binary_encode_unsigned((uint64_t)value, type->bits, output, sizeof(output));
    }
    if (status != BINARY_OK) { fprintf(stderr, "%s\n", binary_status_message(status)); return EXIT_FAILURE; }
    puts(output);
  } else {
    binary_status status;
    if (type->is_signed) { int64_t value; status = binary_decode_signed(argv[3], type->bits, &value); if (status == BINARY_OK) printf("%" PRId64 "\n", value); }
    else { uint64_t value; status = binary_decode_unsigned(argv[3], type->bits, &value); if (status == BINARY_OK) printf("%" PRIu64 "\n", value); }
    if (status != BINARY_OK) { fprintf(stderr, "%s\n", binary_status_message(status)); return EXIT_FAILURE; }
  }
  return EXIT_SUCCESS;
}
