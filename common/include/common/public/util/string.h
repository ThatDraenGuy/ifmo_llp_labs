//
// Created by draen on 27.09.23.
//

#ifndef LLP_LAB_COMMON_INCLUDE_COMMON_PUBLIC_UTIL_STRING_H
#define LLP_LAB_COMMON_INCLUDE_COMMON_PUBLIC_UTIL_STRING_H

#include "result.h"
#include <stdint.h>

typedef struct {
  uint64_t bytes;
} string_size_t;

struct str;
typedef struct str *str_t;

struct string;
typedef struct string *string_t;

#define STR(StrName, String)                                                   \
  static inline str_t StrName() {                                              \
    struct str {                                                               \
      string_size_t size;                                                      \
      char data[];                                                             \
    };                                                                         \
    static struct str __str = {.size = {.bytes = sizeof(String) - 1},          \
                               .data = String};                                \
    return (str_t)&__str;                                                      \
  }

string_t str_into(str_t self);
string_size_t str_len(str_t self);
size_t str_size(str_t self);
result_t str_try_from(void *source, size_t size_limit, str_t *result);
int str_compare(str_t first, str_t second);
bool str_eq(str_t first, str_t second);
const char *str_get_c_string(str_t self);

str_t string_as_str(string_t self);
string_t string_clone(string_t self);
void string_append(string_t self, str_t other);
string_size_t string_len(string_t self);
size_t string_pack_size(string_t self);
void string_pack(string_t self, void *target);
void string_destroy(string_t self);

#endif // LLP_LAB_COMMON_INCLUDE_COMMON_PUBLIC_UTIL_STRING_H
