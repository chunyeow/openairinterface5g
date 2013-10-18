/* For development only : */
%debug
%error-verbose

/* Keep track of location */
%locations
%defines
%pure-parser

%{
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "parser.h"

int
yyerror(const char *msg);

extern int yywrap();
extern int yylex();

extern int yylineno;

/* The Lex parser prototype */
int fddlex(YYSTYPE *lvalp, YYLTYPE *llocp);
%}

/* Values returned by lex for token */
%union {
    char         *string;   /* The string is allocated by strdup in lex.*/
    int           integer;  /* Store integer values */
}

%token LEX_ERROR

%token <string> QSTRING
%token <string> STRING
%token <integer> INTEGER
%token Y_TYPEDEF
%token Y_STRUCT
%token Y_UNION
%token Y_CHARTYPE
%token Y_SHORTTYPE
%token Y_INTTYPE
%token Y_LONGTYPE
%token Y_ENUM
%token Y_UNSIGNED
%token Y_SIGNED
%token Y_EXTENSION
%token Y_ATTRIBUTE
%token Y_ATTRIBUTE_MODE
%token Y_ATTRIBUTE_HI
%token Y_ATTRIBUTE_SI
%token Y_VOIDTYPE
%token Y_POINTER
%token Y_SIZEOF

%%
messages:
    | messages definition
    | messages errors
    {
        yyerror("An error occurred while parsing the configuration file");
        return EINVAL;
    }
    ;

definition:
    | Y_EXTENSION definition
    | Y_TYPEDEF enumerate
    | Y_TYPEDEF structure
    | structure
    | enumerate
    | Y_STRUCT STRING ';'
    | Y_TYPEDEF singletype attribute ';'
    ;

enumerate:
    | Y_ENUM '{' enumlist '}' STRING ';'
    | Y_ENUM STRING '{' enumlist '}' ';'
    | Y_ENUM STRING '{' enumlist '}' STRING ';'
    ;

enumlist:
    | enumitem ',' enumlist
    | enumitem
    ;

enumitem: STRING '=' INTEGER
    | STRING '=' STRING
    | STRING
    ;

structure: Y_STRUCT STRING '{' paramlist '}' STRING ';'
    | Y_STRUCT '{' paramlist '}' STRING ';'
    | Y_STRUCT STRING '{' paramlist '}' ';'
    | Y_STRUCT STRING ';'
    | Y_STRUCT STRING STRING attribute ';'
    | Y_STRUCT singletype ';'
    ;

paramlist:
    | paramlist singletype ';'
    | paramlist union
    | paramlist structure
    ;

union: Y_UNION STRING '{' paramlist '}' STRING ';'
    | Y_UNION '{' paramlist '}' STRING ';'
    | Y_UNION STRING '{' paramlist '}' ';'
    ;

singletype:
    | Y_SIGNED singletype
    | Y_UNSIGNED singletype
    | STRING STRING
    | Y_CHARTYPE STRING
    | Y_SHORTTYPE STRING
    | Y_INTTYPE STRING
    | Y_SHORTTYPE Y_INTTYPE STRING
    | Y_LONGTYPE STRING
    | Y_LONGTYPE Y_INTTYPE STRING
    | Y_LONGTYPE Y_LONGTYPE Y_INTTYPE STRING
    | Y_VOIDTYPE Y_POINTER STRING
    | Y_VOIDTYPE STRING
    | STRING Y_POINTER STRING
    | basictype Y_POINTER STRING
    | singletype '[' arraydef ']'
    | structure
    ;

arraydef:
    | INTEGER arraydef
    | '*' arraydef
    | Y_POINTER arraydef
    | Y_SIZEOF arraydef
    | '(' arraydef
    | ')' arraydef
    | basictype arraydef
    | '-' arraydef
    | STRING arraydef
    ;

attribute:
    | Y_ATTRIBUTE '(' attributevalue ')'
    ;

attributevalue:
    | '(' Y_ATTRIBUTE_MODE attributeparam ')'
    | Y_ATTRIBUTE_MODE attributeparam
    ;

attributeparam:
    | '(' Y_ATTRIBUTE_HI ')'
    | '(' Y_ATTRIBUTE_SI ')'
    ;

basictype:
    | Y_CHARTYPE
    | Y_SHORTTYPE
    | Y_INTTYPE
    | Y_LONGTYPE
    | Y_VOIDTYPE
    ;

        /* Lexical or syntax error */
errors: LEX_ERROR
    | error
    ;

%%

int
yyerror(const char *msg) {
    extern char *yytext;
    fprintf(stderr,
            "Parse error near line %d (token \"%s\"): %s\n",
            yylineno, yytext, msg);
    return -1;
}
