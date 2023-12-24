//
// Created by draen on 26.09.23.
//

#include "database/public/database/domain/record/record_printer.h"
#include "common/public/error/errors_common.h"
#include "database/private/database/domain/record/record.h"
#include <stdio.h>

#define ERROR_SOURCE STR_OF("RECORD_PRINTER")

result_t schema_print(struct table_schema *schema) {
  ASSERT_NOT_NULL(schema, ERROR_SOURCE);

  printf("%s\n", str_get_c_string(table_schema_get_name(schema)));

  struct column_schema_iterator *it = table_schema_get_columns(schema);
  while (column_schema_iterator_has_next(it)) {
    struct column_schema *column_schema = NULL;
    column_schema_iterator_next(it, &column_schema);

    printf("%s\t", str_get_c_string(column_schema_get_name(column_schema)));
  }
  column_schema_iterator_destroy(it);
  printf("\n");
  OK;
}

result_t record_print(struct record *record) {
  ASSERT_NOT_NULL(record, ERROR_SOURCE);

  for (size_t index = 0; index < record->column_schema_group->columns_amount;
       index++) {
    struct column_schema *schema = record->column_schema_group->schemas[index];
    column_value_t value = record->values[index];

    //    printf("%s\t", str_get_c_string(column_schema_get_name(schema)));
    switch (column_schema_get_col_type(schema)) {
    case COLUMN_TYPE_INT32:
      printf("%d\t\t\t", value.int32_value);
      break;
    case COLUMN_TYPE_UINT64:
      printf("%lu\t\t\t", value.uint64_value);
      break;
    case COLUMN_TYPE_FLOAT:
      printf("%f\t\t\t", value.float_value);
      break;
    case COLUMN_TYPE_STRING:
      printf("%s\t\t\t", str_get_c_string(string_as_str(value.string_value)));
      break;
    case COLUMN_TYPE_BOOL:
      printf("%d\t\t\t", value.bool_value);
      break;
    }
  }
  printf("\n");
  OK;
}