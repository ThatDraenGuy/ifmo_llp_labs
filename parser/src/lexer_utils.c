//
// Created by draen on 25.10.23.
//

#include "parser/private/lexer_utils.h"
#include <stdio.h>

void debug_lex(const char *const value, const char *const parsed_value) {
  printf("Found value \"%s\"; parsed value \"%s\"\n", value, parsed_value);
}