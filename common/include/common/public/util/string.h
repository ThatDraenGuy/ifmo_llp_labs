//
// Created by draen on 27.09.23.
//

#ifndef LLP_LAB_COMMON_INCLUDE_COMMON_PUBLIC_UTIL_STRING_H
#define LLP_LAB_COMMON_INCLUDE_COMMON_PUBLIC_UTIL_STRING_H

typedef struct s_str str_t;

#include "result.h"
#include "stdbool.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
  uint64_t bytes;
} string_size_t;

struct s_str {
  string_size_t _size;
  char *_data;
};

typedef struct {
  string_size_t _capacity;
  string_size_t _size;
  char *_data;
} string_t;

#define STR_NULL                                                               \
  (str_t) { ._data = NULL, ._size = 0 }

#define STR_OF(String)                                                         \
  (str_t) { ._size = {.bytes = sizeof(String) - 1}, ._data = String }

str_t str_of(char *c_str);
string_t str_into(str_t self);
string_size_t str_len(str_t self);
size_t str_pack_size(str_t self);
result_t str_try_from(void *source, size_t size_limit, str_t *result);
int str_compare(str_t first, str_t second);
bool str_eq(str_t first, str_t second);
const char *str_get_c_string(str_t self);

#define STRING_NULL                                                            \
  (string_t) { ._data = NULL, ._size = 0, ._capacity = 0 }

static inline bool string_is_null(string_t string) {
  return string._data == NULL;
}

string_t string_from(char *c_str);
str_t string_as_str(string_t self);
string_t string_clone(string_t self);
void string_append(string_t self, str_t other);
string_size_t string_len(string_t self);
size_t string_pack_size(string_t self);
void string_pack(string_t self, void *target);
void string_destroy(string_t self);

#endif // LLP_LAB_COMMON_INCLUDE_COMMON_PUBLIC_UTIL_STRING_H
