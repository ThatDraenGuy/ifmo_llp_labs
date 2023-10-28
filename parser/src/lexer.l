%option noyywrap nodefault yylineno case-insensitive
%{
    #include "parser.tab.h"
    #include "parser/private/lexer_utils.h"

    extern void yyerror(char *fmt, ...);
%}

%%
[ \n\t]+

    /* keywords */
CREATE          { HANDLE_TOKEN(CREATE) }
DELETE          { HANDLE_TOKEN(DELETE) }
DROP            { HANDLE_TOKEN(DROP) }
TABLE           { HANDLE_TOKEN(TABLE) }
SELECT          { HANDLE_TOKEN(SELECT) }
"INSERT INTO"   { HANDLE_TOKEN(INSERT_INTO) }
UPDATE          { HANDLE_TOKEN(UPDATE) }
SET             { HANDLE_TOKEN(SET) }
VALUES          { HANDLE_TOKEN(VALUES) }
FROM            { HANDLE_TOKEN(FROM) }
WHERE           { HANDLE_TOKEN(WHERE) }
ON              { HANDLE_TOKEN(ON) }
JOIN            { HANDLE_TOKEN(JOIN) }

    /* numbers */
-?[0-9]+	    { HANDLE_INT(atoi(yytext)) }
-?[0-9]+"."[0-9]* |
-?"."[0-9]+	|
-?[0-9]+E[-+]?[0-9]+	|
-?[0-9]+"."[0-9]*E[-+]?[0-9]+ |
-?"."[0-9]*E[-+]?[0-9]+ { HANDLE_FLOAT(atof(yytext)) }

    /* booleans */
TRUE            { HANDLE_BOOL(true) }
FALSE           { HANDLE_BOOL(false) }





    /* comparison operators */
"="             { HANDLE_COMPARISON(EQ) }
"!="            { HANDLE_COMPARISON(NEQ) }
"<"             { HANDLE_COMPARISON(LESS) }
"<="            { HANDLE_COMPARISON(LEQ) }
">"             { HANDLE_COMPARISON(GREATER) }
">="            { HANDLE_COMPARISON(GEQ) }


    /* logical operators */
AND             { HANDLE_TOKEN(AND_OP) }
OR              { HANDLE_TOKEN(OR_OP) }
NOT             { HANDLE_TOKEN(NOT_OP) }


    /* arithmetic operators */
"+"             { HANDLE_ARITHMETIC(PLUS) }
"-"             { HANDLE_ARITHMETIC(MINUS) }
"*"             { HANDLE_ARITHMETIC(MUL) }
"/"             { HANDLE_ARITHMETIC(DIV) }

    /* identifiers */
[A-Za-z][A-Za-z0-9_]*   { HANDLE_IDENTIFIER(yytext) }

    /* strings */
'(\\.|''|[^'\n])*'	|
\"(\\.|\"\"|[^"\n])*\" { HANDLE_STRING(yytext) }

    /* misc symbols */
";"                     { HANDLE_SYMBOL(';') }
"."                     { HANDLE_SYMBOL('.') }

.                      { printf("mystery character: %s", yytext); }

%%