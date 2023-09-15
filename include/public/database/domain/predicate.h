//
// Created by draen on 14.09.23.
//

#ifndef LLP_LAB_INCLUDE_PUBLIC_DATABASE_DOMAIN_PREDICATE_H
#define LLP_LAB_INCLUDE_PUBLIC_DATABASE_DOMAIN_PREDICATE_H

#include "data.h"
#include <stdbool.h>

struct predicate;

bool predicate_apply(struct predicate *predicate, struct record *record);

#endif // LLP_LAB_INCLUDE_PUBLIC_DATABASE_DOMAIN_PREDICATE_H
