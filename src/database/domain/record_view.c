//
// Created by draen on 27.09.23.
//

#include "private/database/domain/record_view.h"
#include "public/database/domain/single_record_holder.h"
#include "public/database/record_serialization.h"
#include <malloc.h>

// struct table_schema *record_view_get_schema(struct record_view *self) {
//   return self->schema;
// }

bool record_view_has_next(struct record_view *self) { return !self->is_empty; }

result_t record_view_next(struct record_view *self, struct record **result) {
  struct single_record_holder *temp = self->current_record_holder;
  self->current_record_holder = self->next_record_holder;
  self->next_record_holder = temp;

  *result = single_record_holder_get(self->current_record_holder);

  while (item_iterator_has_next(self->main_item_it)) {
    single_record_holder_clear_all(self->next_record_holder);

    item_t item = ITEM_NULL;
    TRY(item_iterator_next(self->main_item_it, &item));
    CATCH(error, THROW(error))

    TRY(record_deserialize(
        item, single_record_holder_get(self->next_record_holder), 0,
        table_schema_get_column_amount(table_get_schema(self->main_table))));
    CATCH(error, THROW(error))

    size_t column_offset =
        table_schema_get_column_amount(table_get_schema(self->main_table));
    for (size_t join_index = 0; join_index < self->joins_num; join_index++) {
      struct table *join_table = self->join_tables[join_index];
      struct predicate *join_predicate = self->join_predicates[join_index];
      TRY(table_manager_find_one(self->table_manager, join_table,
                                 join_predicate, column_offset,
                                 self->next_record_holder));
      CATCH(error, THROW(error))

      column_offset +=
          table_schema_get_column_amount(table_get_schema(join_table));
    }

    bool predicate_res = false;
    TRY(predicate_apply(self->where,
                        single_record_holder_get(self->next_record_holder),
                        &predicate_res));
    CATCH(error, THROW(error))

    if (predicate_res) {
      OK;
    }
  }
  self->is_empty = true;
  OK;
}

void record_view_destroy(struct record_view *self) {
  predicate_destroy(self->where);
  for (size_t index = 0; index < self->joins_num; index++) {
    //    predicate_destroy(self->join_predicates[index]);
  }
  single_record_holder_destroy(self->current_record_holder);
  single_record_holder_destroy(self->next_record_holder);
  item_iterator_destroy(self->main_item_it);
  free(self);
}