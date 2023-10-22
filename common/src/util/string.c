//
// Created by draen on 27.09.23.
//

#include "common/private/util/string.h"
#include <malloc.h>
#include <string.h>

#define ERROR_SOURCE "STRING"
#define ERROR_TYPE "STRING_ERROR"
enum error_code { INVALID_DATA };
static const char *const error_messages[] = {
    [INVALID_DATA] = "Cannot create string reference from this data!"};

static struct error *error_self(enum error_code error_code) {
  return error_new(ERROR_SOURCE, ERROR_TYPE, (error_code_t){error_code},
                   error_messages[error_code]);
}

static string_t string_new(const char *data, string_size_t size) {
  struct string *string = malloc(sizeof(struct string) + size.bytes + 1);
  memcpy(string->data, data, size.bytes);
  *(string->data + size.bytes) = '\0';
  string->size = size;
  string->capacity = size;
  return (string_t)string;
}

string_t str_into(str_t self) { return string_new(self->data, self->size); }
string_size_t str_len(str_t self) { return self->size; }
size_t str_size(str_t self) { return sizeof(string_size_t) + self->size.bytes; }

result_t str_try_from(void *source, size_t size_limit, str_t *result) {
  str_t str = (str_t)source;
  if (str->size.bytes > size_limit)
    THROW(error_self(INVALID_DATA));

  *result = str;
  OK;
}

int str_compare(str_t first, str_t second) {
  int res = strncmp(first->data, second->data, first->size.bytes);
  return res;
}

bool str_eq(str_t first, str_t second) {
  return str_compare(first, second) == 0;
}

const char *str_get_c_string(str_t self) { return self->data; }

str_t string_as_str(string_t self) {
  return (str_t)((void *)self + offsetof(struct string, size));
}

size_t string_pack_size(string_t self) { return str_size(string_as_str(self)); }

void string_pack(string_t self, void *target) {
  memcpy(target, string_as_str(self), string_pack_size(self));
}

string_t string_clone(string_t self) {
  return string_new(self->data, self->size);
}

void string_destroy(string_t self) { free(self); }