//
// Created by draen on 07.09.23.
//

#ifndef LLP_LAB_INCLUDE_PUBLIC_UTIL_ITERATOR_H
#define LLP_LAB_INCLUDE_PUBLIC_UTIL_ITERATOR_H

#include <stdbool.h>

struct iterator;

bool iterator_has_next(struct iterator *iterator);
void *iterator_next(struct iterator *iterator);

#endif // LLP_LAB_INCLUDE_PUBLIC_UTIL_ITERATOR_H
