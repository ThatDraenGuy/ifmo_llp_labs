//
// Created by draen on 27.09.23.
//

#include "common/private/util/string.h"
#include <malloc.h>
#include <string.h>

#define ERROR_SOURCE STR_OF("STRING")
#define ERROR_TYPE STR_OF("STRING_ERROR")
enum error_code { INVALID_DATA };
static const str_t error_messages[] = {
    [INVALID_DATA] = STR_OF("Cannot create string reference from this data!")};

static struct error *error_self(enum error_code error_code) {
  return error_new(ERROR_SOURCE, ERROR_TYPE, (error_code_t){error_code},
                   error_messages[error_code]);
}

static string_t string_new(const char *data, string_size_t size) {
  char *heap_data = malloc(size.bytes + 1);
  memcpy(heap_data, data, size.bytes + 1);
  return (string_t){._capacity = size, ._size = size, ._data = heap_data};
}
str_t str_of(char *c_str) {
  return (str_t){._size = (string_size_t){strlen(c_str)}, ._data = c_str};
}
string_t str_into(str_t self) { return string_new(self._data, self._size); }
string_size_t str_len(str_t self) { return self._size; }
size_t str_pack_size(str_t self) {
  return sizeof(string_size_t) + self._size.bytes;
}

string_t string_from(char *c_str) {
  return string_new(c_str, (string_size_t){strlen(c_str)});
}

result_t str_try_from(void *source, size_t size_limit, str_t *result) {
  string_size_t size = *(string_size_t *)source;

  if (size.bytes > size_limit)
    THROW(error_self(INVALID_DATA));

  char *data = (source + sizeof(string_size_t));
  *result = (str_t){._size = size, ._data = data};
  OK;
}

int str_compare(str_t first, str_t second) {
  string_size_t size_limit =
      first._size.bytes < second._size.bytes ? first._size : second._size;
  int res = strncmp(first._data, second._data, size_limit.bytes);
  return res;
}

bool str_eq(str_t first, str_t second) {
  return str_compare(first, second) == 0;
}

const char *str_get_c_string(str_t self) { return self._data; }

str_t string_as_str(string_t self) {
  return (str_t){._size = self._size, ._data = self._data};
}

size_t string_pack_size(string_t self) {
  return str_pack_size(string_as_str(self));
}

void string_pack(string_t self, void *target) {
  memcpy(target, &self._size, sizeof(string_size_t));
  memcpy(target + sizeof(string_size_t), self._data, self._size.bytes);
}

string_t string_clone(string_t self) {
  return string_new(self._data, self._size);
}

void string_destroy(string_t self) { free(self._data); }