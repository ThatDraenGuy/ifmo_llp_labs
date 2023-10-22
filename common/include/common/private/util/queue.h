//
// Created by draen on 22.09.23.
//

#ifndef LLP_LAB_COMMON_INCLUDE_COMMON_PRIVATE_UTIL_QUEUE_H
#define LLP_LAB_COMMON_INCLUDE_COMMON_PRIVATE_UTIL_QUEUE_H

#include "common/public/util/queue.h"
#include <stdint.h>

#define DEFAULT_NODE_ENTRIES_AMOUNT 10

struct queue_node {
  struct queue_node *next;
  size_t current_entries_amount;
  uint8_t contents[];
};

struct queue {
  void (*entry_destroy_impl)(void *entry);
  size_t size;
  size_t entry_size;
  size_t node_entries_amount;
  struct queue_node *first_node;
  struct queue_node *last_node;
};

struct queue_iterator {
  struct queue_node *current_node;
  size_t next_index;
  size_t entry_size;
};

#endif // LLP_LAB_COMMON_INCLUDE_COMMON_PRIVATE_UTIL_QUEUE_H
