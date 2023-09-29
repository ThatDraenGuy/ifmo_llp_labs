//
// Created by draen on 22.09.23.
//
#include "private/util/queue.h"
#include "public/error/errors_common.h"
#include <malloc.h>
#include <memory.h>

#define ERROR_SOURCE "QUEUE"

struct queue *queue_new() { return malloc(sizeof(struct queue)); }

void queue_ctor_with_params(struct queue *self, size_t entry_size,
                            size_t node_entries_amount,
                            void (*entry_destroy_impl)(void *entry)) {
  self->entry_size = entry_size;
  self->node_entries_amount = node_entries_amount;
  self->entry_destroy_impl = entry_destroy_impl;
  self->size = 0;
  self->first_node = NULL;
  self->last_node = NULL;
}

void queue_ctor(struct queue *self, size_t entry_size,
                void (*entry_destroy_impl)(void *entry)) {
  queue_ctor_with_params(self, entry_size, DEFAULT_NODE_ENTRIES_AMOUNT,
                         entry_destroy_impl);
}

size_t queue_size(struct queue *self) { return self->size; }

static struct queue_node *node_new(struct queue *self) {
  struct queue_node *node = malloc(
      sizeof(struct queue_node) + self->entry_size * self->node_entries_amount);
  node->next = NULL;
  node->current_entries_amount = 0;
  return node;
}

static void *insert_into_node(struct queue_node *node, size_t entry_size,
                              void *entry) {
  void *data_destination =
      node->contents + entry_size * node->current_entries_amount;
  if (entry != NULL) {
    memcpy(data_destination, entry, entry_size);
  }
  node->current_entries_amount++;
  return data_destination;
}

void *queue_push_back(struct queue *self, void *entry) {
  self->size++;

  struct queue_node *last_node = self->last_node;
  if (last_node == NULL) {
    // if entry is the very first entry
    struct queue_node *new_node = node_new(self);
    self->first_node = new_node;
    self->last_node = new_node;
    return insert_into_node(new_node, self->entry_size, entry);
  } else {
    if (last_node->current_entries_amount < self->node_entries_amount) {
      // if we have space in last node
      return insert_into_node(last_node, self->entry_size, entry);
    } else {
      // if no space in last node
      struct queue_node *new_node = node_new(self);
      last_node->next = new_node;
      self->last_node = new_node;
      return insert_into_node(new_node, self->entry_size, entry);
    }
  }
}

result_t queue_get(struct queue *self, size_t index, void **entry) {
  if (self->size <= index)
    THROW(error_common(ERROR_SOURCE, ERR_INDEX_OUT_OF_BOUNDS));

  size_t node_index = index / self->node_entries_amount;
  size_t entry_index = index % self->node_entries_amount;

  struct queue_node *node = self->first_node;
  for (size_t i = 0; i < node_index; i++) {
    node = node->next;
  }

  *entry = node->contents + entry_index * self->entry_size;
  OK;
}
void queue_clear(struct queue *self) {
  struct queue_node *node = self->first_node;
  while (node != NULL) {
    struct queue_node *next_node = node->next;
    for (size_t index = 0; index < node->current_entries_amount; index++) {
      self->entry_destroy_impl(node->contents + index * self->entry_size);
    }
    free(node);
    node = next_node;
  }
  self->size = 0;
  self->first_node = NULL;
  self->last_node = NULL;
}
void queue_destroy(struct queue *self) {
  queue_clear(self);
  free(self);
}

struct queue_iterator *queue_get_entries(struct queue *self) {
  struct queue_iterator *it = malloc(sizeof(struct queue_iterator));
  it->entry_size = self->entry_size;
  it->next_index = 0;
  it->current_node = self->first_node;
  return it;
}

bool queue_iterator_has_next(struct queue_iterator *self) {
  // if we have an iterator over empty queue
  if (self->current_node == NULL)
    return false;
  // if there are items left in the node - true
  if (self->next_index < self->current_node->current_entries_amount)
    return true;

  // otherwise see if there is next node
  return self->current_node->next != NULL;
}

result_t queue_iterator_next(struct queue_iterator *self, void **entry) {
  ASSERT_NOT_NULL(self, ERROR_SOURCE);
  if (!queue_iterator_has_next(self))
    THROW(error_common(ERROR_SOURCE, ERR_COMMON_ITER_OUT_OF_RANGE));

  if (self->next_index >= self->current_node->current_entries_amount) {
    self->current_node = self->current_node->next;
    self->next_index = 0;
  }

  *entry = self->current_node->contents + self->next_index * self->entry_size;
  self->next_index++;
  OK;
}

void queue_iterator_destroy(struct queue_iterator *self) { free(self); }