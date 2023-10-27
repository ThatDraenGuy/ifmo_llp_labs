//
// Created by draen on 22.10.23.
//

#ifndef LLP_LAB_COMMON_INCLUDE_COMMON_PUBLIC_DOMAIN_EXPRESSION_OPERATOR_TYPES_H
#define LLP_LAB_COMMON_INCLUDE_COMMON_PUBLIC_DOMAIN_EXPRESSION_OPERATOR_TYPES_H

typedef enum {
  ARITHMETIC_OP,
  COMPARISON_OP,
  LOGICAL_OP
} expression_operator_type_t;
typedef enum { PLUS, MINUS, MUL, DIV } arithmetic_operator_t;
typedef enum { EQ, NEQ, LESS, LEQ, GREATER, GEQ } comparison_operator_t;
typedef enum { AND, OR, NOT } logical_operator_t;

#endif // LLP_LAB_COMMON_INCLUDE_COMMON_PUBLIC_DOMAIN_EXPRESSION_OPERATOR_TYPES_H
