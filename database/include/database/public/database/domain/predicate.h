//
// Created by draen on 14.09.23.
//

#ifndef LLP_LAB_INCLUDE_PUBLIC_DATABASE_DOMAIN_PREDICATE_H
#define LLP_LAB_INCLUDE_PUBLIC_DATABASE_DOMAIN_PREDICATE_H

#include "database/public/database/domain/expression/expression_interface.h"
#include "database/public/database/domain/record/record_group.h"
#include <stdbool.h>
#include <stdint.h>



struct predicate;

struct predicate *predicate_of(struct i_expression *expression);

result_t predicate_apply(struct predicate *self, struct record *record,
                         bool *result);
struct predicate *predicate_clone(struct predicate *self);

void predicate_destroy(struct predicate *self);

#endif // LLP_LAB_INCLUDE_PUBLIC_DATABASE_DOMAIN_PREDICATE_H
