//
// Created by draen on 25.10.23.
//
#include "parser/private/ast_node_interface.h"
#include <stddef.h>

bool ast_node_equals(struct i_ast_node *self, struct i_ast_node *other) {
  if (self == NULL && other == NULL)
    return true;
  if (self == NULL || other == NULL)
    return false;
  return self->equals_impl(self, other);
}

void ast_node_print_at_level(struct i_ast_node *self, size_t level) {
  if (self == NULL)
    return;
  self->print_at_level_impl(self, level);
}

void ast_node_print(struct i_ast_node *self) {
  if (self == NULL)
    return;
  ast_node_print_at_level(self, 0);
}
void ast_node_destroy(struct i_ast_node *self) {
  if (self == NULL)
    return;
  self->destroy_impl(self);
}