%{
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "parser/private/parser.h"
#include "parser/private/ast_nodes.h"
#include "lexer.tab.h"

void yyerror(struct parser_ctx *ctx, char *s, ...);
void yy_set_input_string(const char* in);
void yy_end_lexical_scan();
%}

%parse-param { struct parser_ctx *ctx }
%define parse.error detailed

%code requires {
    #include "parser/private/parser.h"
    int parse_chars(struct parser_ctx *ctx, const char* in);
}

%union {
    struct i_ast_node *node;
}
%start statement
%token <node> CREATE
%token <node> DELETE
%token <node> DROP
%token <node> TABLE
%token <node> SELECT
%token <node> JOIN
%token <node> ON
%token <node> INSERT_INTO
%token <node> UPDATE
%token <node> SET
%token <node> VALUES
%token <node> FROM
%token <node> WHERE
%token <node> INT
%token <node> FLOAT
%token <node> STRING
%token <node> BOOL
%token <node> IDENTIFIER
%token <node> ARITHMETIC_OPERATOR

%left <node> AND_OP
%left <node> OR_OP
%left <node> NOT_OP
%left <node> COMPARISON_OPERATOR


%type <node> statement select_statement insert_statement update_statement delete_statement updates update values_list values from joins join where predicate expression literal member_id table_id column_id

%%

statement: select_statement ';' {
    $$ = ast_node_stmt_new($1);
    ctx->root = $$;
    ctx->is_error = false;
    YYACCEPT;
} | insert_statement ';' {
     $$ = ast_node_stmt_new($1);
     ctx->root = $$;
     ctx->is_error = false;
     YYACCEPT;
} | update_statement ';' {
      $$ = ast_node_stmt_new($1);
      ctx->root = $$;
      ctx->is_error = false;
      YYACCEPT;
 } | delete_statement ';' {
    $$ = ast_node_stmt_new($1);
    ctx->root = $$;
    ctx->is_error = false;
    YYACCEPT;
};

select_statement: SELECT from joins where {
    $$ = ast_node_select_stmt_new($2, $3, $4);
} | SELECT from where {
    $$ = ast_node_select_stmt_new($2, NULL, $3);
} | SELECT from joins {
    $$ = ast_node_select_stmt_new($2, $3, NULL);
} | SELECT from {
    $$ = ast_node_select_stmt_new($2, NULL, NULL);
};

insert_statement: INSERT_INTO table_id VALUES values_list {
    $$ = ast_node_insert_stmt_new($2, $4);
}

update_statement: UPDATE table_id SET updates where {
    $$ = ast_node_update_stmt_new($2, $4, $5);
} | UPDATE table_id SET updates {
    $$ = ast_node_update_stmt_new($2, $4, NULL);
}

delete_statement: DELETE from where {
    $$ = ast_node_delete_stmt_new($2, $3);
} | DELETE from {
    $$ = ast_node_delete_stmt_new($2, NULL);
};

updates: update {
    $$ = ast_node_updates_new();
    ast_node_updates_add((struct ast_node_updates *)$$, $1);
} | updates ',' update {
    ast_node_updates_add((struct ast_node_updates *)$$, $3);
}

update: column_id COMPARISON_OPERATOR expression {
    if (((struct ast_node_comparison_oper *)$2)->oper != EQ) {
        yyerror(ctx, "bad comparison MAYBE FIX ME");
        YYABORT;
    }
    $$ = ast_node_update_new($1, $3);
}

values_list: '(' values ')' {
    $$ = ast_node_values_list_new();
    ast_node_values_list_add((struct ast_node_values_list *)$$, $2);
} | values_list ',' '(' values ')' {
    ast_node_values_list_add((struct ast_node_values_list *)$$, $4);
};

values: literal {
    $$ = ast_node_values_new();
    ast_node_values_add((struct ast_node_values *)$$, $1);
} | values ',' literal {
    ast_node_values_add((struct ast_node_values *)$$, $3);
};

from: FROM table_id {
    $$ = ast_node_from_new($2);
};

joins: join {
    $$ = ast_node_joins_new();
    ast_node_joins_add((struct ast_node_joins *)$$, $1);
} | joins join {
    ast_node_joins_add((struct ast_node_joins *)$$, $2);
};

join: JOIN table_id ON predicate {
    $$ = ast_node_join_on_new($2, $4);
};

where: WHERE predicate {
    $$ = ast_node_where_new($2);
};

predicate: expression {
    $$ = ast_node_predicate_new($1);
};

expression: literal {
    $$ = ast_node_literal_expr_new($1);
} | member_id {
    $$ = ast_node_member_expr_new($1);
} | expression AND_OP expression {
    $$ = ast_node_operator_expr_new($1, $2, $3);
} | expression OR_OP expression {
    $$ = ast_node_operator_expr_new($1, $2, $3);
} | NOT_OP expression {
    $$ = ast_node_operator_expr_new($2, $1, NULL);
} | expression COMPARISON_OPERATOR expression {
    $$ = ast_node_operator_expr_new($1, $2, $3);
} | expression ARITHMETIC_OPERATOR expression {
    $$ = ast_node_operator_expr_new($1, $2, $3);
} | '(' expression ')' {
    $$ = $2;
};

literal: INT {
    $$ = $1;
} | FLOAT {
    $$ = $1;
}| STRING {
    $$ = $1;
} | BOOL {
    $$ = $1;
};

member_id: table_id '.' column_id {
    $$ = ast_node_member_id_new($1, $3);
};

table_id: IDENTIFIER {
    $$ = ast_node_table_id_new($1);
};

column_id: IDENTIFIER {
    $$ = ast_node_column_id_new($1);
};


%%
void
yyerror(struct parser_ctx *ctx, char *fmt, ...)
{
  #define ERROR_SOURCE STR_OF("PARSER")
  #define ERROR_TYPE STR_OF("PARSER_ERROR")

  va_list args;
  va_start(args, fmt);
  char *buf = malloc(sizeof(char) * 100);
  vsprintf(buf, fmt, args);

  ctx->error = error_new(ERROR_SOURCE, ERROR_TYPE, (error_code_t){0}, str_of(buf));
  free(buf);
  ctx->is_error = true;
}

int parse_chars(struct parser_ctx *ctx, const char* in) {
  yy_set_input_string(in);
  int rv = yyparse(ctx);
  yy_end_lexical_scan();
  return rv;
}