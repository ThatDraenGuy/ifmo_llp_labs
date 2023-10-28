//
// Created by draen on 25.10.23.
//

#include "common/public/error/error_handler.h"
#include "parser/prelude.h"
#include <stddef.h>

int main() {
  struct i_ast_node *tree = NULL;
  TRY(parse_stdin(&tree));
  CATCH(error, {
    handle_error(error);
    return 1;
  })

  ast_node_print(tree);
  ast_node_destroy(tree);
  return 0;
}