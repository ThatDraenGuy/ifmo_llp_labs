//
// Created by draen on 07.09.23.
//

#ifndef LLP_LAB_INCLUDE_PRIVATE_UTIL_ITERATOR_H
#define LLP_LAB_INCLUDE_PRIVATE_UTIL_ITERATOR_H

#include "public/util/iterator.h"

struct iterator {
  bool (*iterator_has_next_impl)(struct iterator *this);
  void *(*iterator_next_impl)(struct iterator *this);
};

#endif // LLP_LAB_INCLUDE_PRIVATE_UTIL_ITERATOR_H
