//
// Created by draen on 06.09.23.
//

#include "public/util/linked_list.h"
#include "private/util/linked_list.h"
#include <malloc.h>

void linked_list_node_destroy(struct linked_list_node *this) { free(this); }

struct linked_list *linked_list_new() {
  struct linked_list *list = malloc(sizeof(struct linked_list));
  list->first = NULL;
  list->last = NULL;
  list->size = 0;
  return list;
}

void linked_list_push_back(struct linked_list *linked_list, void *data) {
  if (linked_list == NULL)
    return;

  struct linked_list_node *node = malloc(sizeof(struct linked_list_node));
  node->data = data;
  node->next = NULL;

  if (linked_list->first == NULL) {
    linked_list->first = node;
    linked_list->last = node;
  } else {
    linked_list->last->next = node;
    linked_list->last = node;
  }
  linked_list->size++;
}

void linked_list_destroy(struct linked_list *linked_list) {
  if (linked_list == NULL)
    return;

  struct linked_list_node *node = linked_list->first;
  while (node != NULL) {
    struct linked_list_node *next = node->next;
    linked_list->destroy_node(node);
    node = next;
  }

  free(linked_list);
}