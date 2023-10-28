%{
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "parser/private/parser.h"
#include "parser/public/ast_nodes.h"
#include "lexer.tab.h"

void yyerror(struct parser_ctx *ctx, char *s, ...);
%}

%parse-param { struct parser_ctx *ctx }

%code requires {
    #include "parser/private/parser.h"
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
%token <node> COMPARISON_OPERATOR
%token <node> AND_OP
%token <node> OR_OP
%token <node> NOT_OP
%token ';'

%type <node> statement select_statement from joins join where predicate expression expression_operator literal member_id table_id column_id

%%

statement: select_statement ';' {
    $$ = ast_node_stmt_new($1);
    ctx->root = $$;
    ctx->is_error = false;
    YYACCEPT;
};

select_statement: SELECT from joins where {
    $$ = ast_node_select_stmt_new($2, $3, $4);
} | SELECT from where {
    $$ = ast_node_select_stmt_new($2, NULL, $3);
} | SELECT from {
    $$ = ast_node_select_stmt_new($2, NULL, NULL);
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

expression: expression expression_operator expression {
    $$ = ast_node_operator_expr_new($1, $2, $3);
} | expression AND_OP expression {
    $$ = ast_node_operator_expr_new($1, $2, $3);
} | expression OR_OP expression {
    $$ = ast_node_operator_expr_new($1, $2, $3);
} | NOT_OP expression {
    $$ = ast_node_operator_expr_new($2, $1, NULL);
} | literal {
    $$ = $1;
} | member_id {
    $$ = ast_node_member_expr_new($1);
};

expression_operator: ARITHMETIC_OPERATOR {
    $$ = ast_node_expression_oper_new($1);
} | COMPARISON_OPERATOR {
    $$ = ast_node_expression_oper_new($1);
}

literal: INT {
    $$ = ast_node_literal_expr_new($1);
} | FLOAT {
     $$ = ast_node_literal_expr_new($1);
}| STRING {
    $$ = ast_node_literal_expr_new($1);
} | BOOL {
    $$ = ast_node_literal_expr_new($1);
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
  #define ERROR_SOURCE "PARSER"
  #define ERROR_TYPE "PARSER_ERROR"
  // extern int yylineno;
  //
  // va_list ap;
  // va_start(ap, s);
  //
  // fprintf(stderr, "Error at line %d: error: ", yylineno);
  // vfprintf(stderr, s, ap);
  // fprintf(stderr, "\n");

  va_list args;
  va_start(args, fmt);
  char *buf = malloc(sizeof(char) * 100);
  vsprintf(buf, fmt, args);

  ctx->error = error_new(ERROR_SOURCE, ERROR_TYPE, (error_code_t){0}, buf);
  ctx->is_error = true;
}