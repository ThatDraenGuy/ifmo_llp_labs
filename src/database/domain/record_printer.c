//
// Created by draen on 26.09.23.
//

#include "public/database/domain/record_printer.h"
#include "private/database/domain/record.h"
#include "public/error/errors_common.h"
#include <stdio.h>

#define ERROR_SOURCE "RECORD_PRINTER"

result_t record_print(struct record *record) {
  ASSERT_NOT_NULL(record, ERROR_SOURCE);
  struct queue_iterator *it = queue_get_entries(record->entries);

  while (queue_iterator_has_next(it)) {
    struct record_entry *entry = NULL;
    TRY(queue_iterator_next(it, (void **)&entry));
    CATCH(error, {
      queue_iterator_destroy(it);
      THROW(error);
    })

    printf("%s\t", entry->schema.name);
    switch (entry->schema.type) {
    case COLUMN_TYPE_INT32:
      printf("INT32\t%d\n", entry->value.int32_value);
      break;
    case COLUMN_TYPE_UINT64:
      printf("UINT64\t%lu\n", entry->value.uint64_value);
      break;
    case COLUMN_TYPE_FLOAT:
      printf("FLOAT\t%f\n", entry->value.float_value);
      break;
    case COLUMN_TYPE_STRING:
      printf("STRING\t%s\n", entry->value.string_value);
      break;
    case COLUMN_TYPE_BOOL:
      printf("BOOL\t%d\n", entry->value.bool_value);
      break;
    }
  }
  printf("\n");
  queue_iterator_destroy(it);
  OK;
}