//
// Created by draen on 21.10.23.
//

#ifndef LLP_LAB_PARSER_INCLUDE_PRIVATE_LEXER_UTILS_H
#define LLP_LAB_PARSER_INCLUDE_PRIVATE_LEXER_UTILS_H

#include "parser/public/ast_nodes.h"

#define HANDLE_TOKEN(Token)                                                    \
  debug_lex(yytext, #Token);                                                   \
  return Token;

#define HANDLE_SYMBOL(Symbol)                                                  \
  debug_lex(yytext, "Symbol");                                                 \
  return Symbol;

#define HANDLE_COMPARISON(Operator)                                            \
  debug_lex(yytext, #Operator);                                                \
  yylval.node = ast_node_comparison_oper_new(Operator);                        \
  return COMPARISON_OPERATOR;

#define HANDLE_LOGICAL(Operator, LexOperator)                                  \
  debug_lex(yytext, #LexOperator);                                             \
  yylval.node = ast_node_logical_oper_new(Operator);                           \
  return LexOperator;

#define HANDLE_ARITHMETIC(Operator)                                            \
  debug_lex(yytext, #Operator);                                                \
  yylval.node = ast_node_arithmetic_oper_new(Operator);                        \
  return ARITHMETIC_OPERATOR;

#define HANDLE_INT(Int)                                                        \
  debug_lex(yytext, "Int");                                                    \
  yylval.node = ast_node_int_new(Int);                                         \
  return INT;

#define HANDLE_FLOAT(Float)                                                    \
  debug_lex(yytext, "Float");                                                  \
  yylval.node = ast_node_float_new(Float);                                     \
  return FLOAT;

#define HANDLE_BOOL(Bool)                                                      \
  debug_lex(yytext, "Bool");                                                   \
  yylval.node = ast_node_bool_new(Bool);                                       \
  return BOOL;

#define HANDLE_STRING(Chars)                                                   \
  debug_lex(yytext, "String");                                                 \
  yylval.node = ast_node_string_new(Chars);                                    \
  return STRING;

#define HANDLE_IDENTIFIER(Text)                                                \
  debug_lex(yytext, "Id");                                                     \
  yylval.node = ast_node_id_new(Text);                                         \
  return IDENTIFIER;

void debug_lex(const char *value, const char *parsed_value);

#endif // LLP_LAB_PARSER_INCLUDE_PRIVATE_LEXER_UTILS_H
