//
// Created by draen on 06.09.23.
//

#ifndef LLP_LAB_INCLUDE_PRIVATE_UTIL_LINKED_LIST_H
#define LLP_LAB_INCLUDE_PRIVATE_UTIL_LINKED_LIST_H

#include <stddef.h>

struct linked_list_node {
  void *data;
  struct linked_list_node *next;
};

void linked_list_node_destroy(struct linked_list_node *this);

struct linked_list {
  struct linked_list_node *first;
  struct linked_list_node *last;
  size_t size;

  void (*destroy_node)(struct linked_list_node *this);
};

struct linked_list *linked_list_new();

#endif // LLP_LAB_INCLUDE_PRIVATE_UTIL_LINKED_LIST_H
