//
// Created by draen on 22.09.23.
//

#ifndef LLP_LAB_COMMON_INCLUDE_COMMON_PUBLIC_UTIL_QUEUE_H
#define LLP_LAB_COMMON_INCLUDE_COMMON_PUBLIC_UTIL_QUEUE_H

#include "result.h"
#include <stddef.h>

struct queue;

struct queue *queue_new();
void queue_ctor(struct queue *self, size_t entry_size,
                void (*entry_destroy_impl)(void *entry));
void queue_ctor_with_params(struct queue *self, size_t entry_size,
                            size_t node_entries_amount,
                            void (*entry_destroy_impl)(void *entry));

size_t queue_size(struct queue *self);

void *queue_push_back(struct queue *self, void *entry);
result_t queue_get(struct queue *self, size_t index, void **entry);

void queue_clear(struct queue *self);
void queue_destroy(struct queue *self);

struct queue_iterator;

struct queue_iterator *queue_get_entries(struct queue *self);
bool queue_iterator_has_next(struct queue_iterator *self);
result_t queue_iterator_next(struct queue_iterator *self, void **entry);
void queue_iterator_destroy(struct queue_iterator *self);

#endif // LLP_LAB_COMMON_INCLUDE_COMMON_PUBLIC_UTIL_QUEUE_H
