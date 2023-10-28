//
// Created by draen on 25.10.23.
//

#include "parser/private/parser.h"
#include "parser.tab.h"

result_t parse_stdin(struct i_ast_node **result) {
  struct parser_ctx ctx = {0};
  yyparse(&ctx);

  if (ctx.is_error) {
    THROW(ctx.error);
  } else {
    *result = ctx.root;
    OK;
  }
}

result_t parse_string(str_t str, struct i_ast_node **result) {
  struct parser_ctx ctx = {0};
  parse_chars(&ctx, str._data);

  if (ctx.is_error) {
    THROW(ctx.error);
  } else {
    *result = ctx.root;
    OK;
  }
}
