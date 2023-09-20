#include <malloc.h>
#include <stdio.h>

#include "private/database/domain/table.h"
#include "private/error/error.h"
#include "public/database/table_manager.h"

static void handle_error(struct error *err) {
  printf("Encountered error at %s:\n %s(%zu): %s", err->error_source,
         err->error_type, err->error_code.bytes, err->error_message);
  error_destroy(err);
}

int create_table() {
  struct table_manager *table_manager = table_manager_new();
  TRY(table_manager_ctor(table_manager, "test_file"));
  CATCH(error, {
    handle_error(error);
    return 0;
  })
  struct table_schema *schema = table_schema_new();
  table_schema_ctor(schema, "TEST_TABLE", 3);
  table_schema_add_column(schema, "TEST_INT", COLUMN_TYPE_INT32);
  table_schema_add_column(schema, "TEST_BOOL", COLUMN_TYPE_BOOL);
  table_schema_add_column(schema, "TEST_FLOAT", COLUMN_TYPE_FLOAT);

  struct table *test_table = NULL;
  TRY(table_manager_create_table(table_manager, schema, &test_table));
  CATCH(error, {
    table_manager_destroy(table_manager);
    handle_error(error);
    return 0;
  })

  printf("%zu\n", test_table->page_group_id.bytes);

  TRY(table_manager_flush(table_manager, test_table));
  CATCH(error, {
    table_manager_destroy(table_manager);
    table_destroy(test_table);
    handle_error(error);
    return 0;
  })

  table_manager_destroy(table_manager);
  table_destroy(test_table);
  return 0;
}

int read_table() {
  struct table_manager *table_manager = table_manager_new();
  TRY(table_manager_ctor(table_manager, "test_file"));
  CATCH(error, {
    handle_error(error);
    return 0;
  })

  struct table *test_table = NULL;
  TRY(table_manager_get_table(table_manager, "TEST_TABLE", &test_table));
  CATCH(error, {
    table_manager_destroy(table_manager);
    handle_error(error);
    return 0;
  })

  printf("%zu\n", test_table->page_group_id.bytes);

  table_manager_destroy(table_manager);
  table_destroy(test_table);
  return 0;
}

int main() {

  int res = create_table();
  printf("done");
  return res;
}
