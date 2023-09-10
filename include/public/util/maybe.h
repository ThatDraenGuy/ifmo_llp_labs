//
// Created by draen on 07.09.23.
//

#ifndef LLP_LAB_INCLUDE_PUBLIC_UTIL_MAYBE_H
#define LLP_LAB_INCLUDE_PUBLIC_UTIL_MAYBE_H

#include <stdbool.h>

// macro for a simple maybe_ type.
// maybe_ type if defined by a bool, signifying existence of value
// and value itself
#define maybe(type, name)                                                      \
  typedef struct maybe_##name {                                                \
    bool is_present;                                                           \
    type value;                                                                \
  } maybe_##type;                                                              \
  maybe_##type maybe_##name##_none() {                                         \
    return (struct maybe_##name){.is_present = false};                         \
  }
#endif // LLP_LAB_INCLUDE_PUBLIC_UTIL_MAYBE_H
