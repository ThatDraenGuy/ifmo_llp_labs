//
// Created by draen on 03.10.23.
//

#ifndef LLP_LAB_INCLUDE_PUBLIC_DATABASE_DOMAIN_EXPRESSION_OPERATORS_H
#define LLP_LAB_INCLUDE_PUBLIC_DATABASE_DOMAIN_EXPRESSION_OPERATORS_H

#include "expression_operator_interface.h"

typedef enum { PLUS, MINUS, MUL, DIV } arithmetic_operator_t;
struct i_expression_operator *
arithmetic_operator(arithmetic_operator_t arithmetic_operator,
                    column_type_t operand_type);

typedef enum { EQ, NEQ, LESS, LEQ, GREATER, GEQ } comparison_operator_t;
struct i_expression_operator *
comparison_operator(comparison_operator_t comparison_operator,
                    column_type_t operand_type);

typedef enum { AND, OR, NOT } logical_operator_t;
struct i_expression_operator *
logical_operator(logical_operator_t logical_operator);

#endif // LLP_LAB_INCLUDE_PUBLIC_DATABASE_DOMAIN_EXPRESSION_OPERATORS_H
