//
// Created by draen on 06.09.23.
//

#ifndef LLP_LAB_INCLUDE_PRIVATE_UTIL_LINKED_LIST_H
#define LLP_LAB_INCLUDE_PRIVATE_UTIL_LINKED_LIST_H

#include "public/util/linked_list.h"
#include <stddef.h>

struct linked_list_node {
  void *data;
  struct linked_list_node *next;
};

struct linked_list {
  struct linked_list_node *first;
  struct linked_list_node *last;
  size_t size;

  void (*destroy_node_data_impl)(void *node_data);
};

void linked_list_ctor(struct linked_list *linked_list);

struct list_iterator {
  size_t size;
  size_t index;
  struct linked_list_node *current;
};
#endif // LLP_LAB_INCLUDE_PRIVATE_UTIL_LINKED_LIST_H
