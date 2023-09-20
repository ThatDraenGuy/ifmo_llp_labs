//
// Created by draen on 14.09.23.
//

#ifndef LLP_LAB_INCLUDE_PUBLIC_DATABASE_DOMAIN_RECORD_ITERATOR_H
#define LLP_LAB_INCLUDE_PUBLIC_DATABASE_DOMAIN_RECORD_ITERATOR_H

struct record_iterator;

bool record_iterator_has_next(struct record_iterator *self);

result_t record_iterator_next(struct record_iterator *self,
                              struct record **result);

void record_iterator_destroy(struct record_iterator *self);

#endif // LLP_LAB_INCLUDE_PUBLIC_DATABASE_DOMAIN_RECORD_ITERATOR_H
