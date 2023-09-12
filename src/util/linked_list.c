//
// Created by draen on 06.09.23.
//

#include "public/util/linked_list.h"
#include "private/util/linked_list.h"
#include <malloc.h>
#include <stdbool.h>

void linked_list_ctor(struct linked_list *list) {
  list->first = NULL;
  list->last = NULL;
  list->size = 0;
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
    linked_list->destroy_node_data_impl(node->data);
    free(node);
    node = next;
  }

  free(linked_list);
}

bool linked_list_iterator_has_next(void *this) {
  struct list_iterator *it = this;
  return it->index < it->size;
}

void *linked_list_iterator_next(void *this) {
  struct list_iterator *it = this;
  if (!iterator_has_next((struct i_iterator *)it))
    return NULL;

  void *result = it->current->data;
  it->index++;
  it->current = it->current->next;
  return result;
}

void linked_list_iterator_destroy(void *this) { free(this); }

struct list_iterator *list_iterator(struct linked_list *linked_list) {
  if (linked_list == NULL)
    return NULL;

  struct list_iterator *it = malloc(sizeof(struct list_iterator));
  it->size = linked_list->size;
  it->index = 0;
  it->current = linked_list->first;

  it->parent.iterator_has_next_impl = linked_list_iterator_has_next;
  it->parent.iterator_next_impl = linked_list_iterator_next;
  it->parent.iterator_destroy_impl = linked_list_iterator_destroy;

  return (struct i_iterator *)it;
}