//
// Created by draen on 29.09.23.
//

#ifndef LLP_LAB_INCLUDE_PUBLIC_DATABASE_DOMAIN_JOIN_H
#define LLP_LAB_INCLUDE_PUBLIC_DATABASE_DOMAIN_JOIN_H

#include "common/public/util/string.h"
#include "predicate.h"

struct join;

struct join *join_of(str_t what, struct predicate *on);
str_t join_get_what(struct join *self);
struct predicate *join_get_on(struct join *self);
void join_destroy(struct join *self);

#endif // LLP_LAB_INCLUDE_PUBLIC_DATABASE_DOMAIN_JOIN_H
