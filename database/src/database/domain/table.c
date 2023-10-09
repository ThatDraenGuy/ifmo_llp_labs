//
// Created by draen on 20.09.23.
//

#include "private/database/domain/table.h"
#include <malloc.h>

struct table *table_new() { return malloc(sizeof(struct table)); }

void table_ctor(struct table *self, struct table_schema *schema,
                page_group_id_t page_group_id) {
  self->table_schema = schema;
  self->page_group_id = page_group_id;
}

struct table_schema *table_get_schema(struct table *self) {
  return self->table_schema;
}

void table_destroy(struct table *self) {
  table_schema_destroy(self->table_schema);
  free(self);
}