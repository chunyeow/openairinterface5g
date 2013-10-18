
/* Lex configuration parser.
 *
 * This file defines the token for parsing the configuration file
 *
 * Note : This module is NOT thread-safe. All processing must be done from one thread only.
 */
%{
#include <stdio.h>

/* Include yacc tokens definitions */
#include "parser.h"

/* Update the column information */
#ifdef DEBUG_LEX
#define YY_USER_ACTION {                        \
    yylloc->first_column = yylloc->last_column + 1;         \
    yylloc->last_column = yylloc->first_column + yyleng - 1;    \
    TRACE_DEBUG(FULL,                       \
        "(%d:%d-%d:%d) matched rule %d, length=%d, txt='%s'\n", \
        yylloc->first_line, yylloc->first_column,       \
        yylloc->last_line, yylloc->last_column,         \
        yy_act, yyleng, yytext);                \
}
#else /* DEBUG_LEX */
#define YY_USER_ACTION {                        \
    yylloc->first_column = yylloc->last_column + 1;         \
    yylloc->last_column = yylloc->first_column + yyleng - 1;    \
}
#endif

#define YY_NO_INPUT
%}

%option bison-bridge bison-locations
%option noyywrap
%option nounput
%option yylineno

/* Quoted string. Multilines do not match. */
qstring     \"[^\"\n]*\"

%%

    /* List of patterns and actions */

<*>\n {
    /* Update the line count */
    yylloc->first_line++;
    yylloc->last_line++;
    yylloc->last_column=0;
}

<*>([[:space:]]{-}[\n])+    ; /* Eat all spaces, not new lines */
<*>#.*$                     ; /* Eat all comments */

    /* Full words tokens (keywords) */
(?i:"typedef") { return Y_TYPEDEF; }
(?i:"struct")  { return Y_STRUCT; }
(?i:"union")   { return Y_UNION; }
(?i:"enum")    { return Y_ENUM; }
(?i:"signed")      { return Y_SIGNED; }
(?i:"unsigned")      { return Y_UNSIGNED; }
(?i:"short")    { return Y_SHORTTYPE; }
(?i:"char")     { return Y_CHARTYPE; }
(?i:"int")           { return Y_INTTYPE; }
(?i:"long")           { return Y_LONGTYPE; }
(?i:"void")           { return Y_VOIDTYPE; }
(?i:"__extension__") { return Y_EXTENSION; }
(?i:"__attribute__") { return Y_ATTRIBUTE; }
(?i:"__mode__") { return Y_ATTRIBUTE_MODE; }
(?i:"__HI__") { return Y_ATTRIBUTE_HI; }
(?i:"__SI__") { return Y_ATTRIBUTE_SI; }
(?i:"sizeof") { return Y_SIZEOF; }

[A-Za-z_][A-Za-z0-9_]*       {
    /* First copy the string without the quotes for use in the yacc parser */
    if ((yylval->string = strdup(yytext+1)) == NULL) { /* This allocates one useless tail char but... it's easier :D */
        return LEX_ERROR;/* on error, trig an error in yacc parser */
    }
    yylval->string[yyleng-2] = '\0';

    /* the yacc parser will check the string is valid */
    return STRING;
}

{qstring}       {
    /* First copy the string without the quotes for use in the yacc parser */
    if ((yylval->string = strdup(yytext+1)) == NULL) { /* This allocates one useless tail char but... it's easier :D */
        return LEX_ERROR;/* on error, trig an error in yacc parser */
    }
    yylval->string[yyleng-2] = '\0';

    /* the yacc parser will check the string is valid */
    return QSTRING;
}

[[:digit:]]+    {
    /* Convert this to an integer value */
    int ret = sscanf(yytext, "%i", &yylval->integer);
    if (ret != 1) {
        /* No matching: an error occurred */
        fprintf(stderr, "Unable to convert the value '%s' to a valid number: %s\n",
                yytext, strerror(errno));
        return LEX_ERROR; /* trig an error in yacc parser */
        /* Maybe we could REJECT instead of failing here? */
    }
    return INTEGER;
}

    /* Valid single characters for yyparse */
<*>[\-+=,:;{}\[\]\(\)]     { return yytext[0]; }
<*>[*]*         { return Y_POINTER; }

<*>[[:alnum:]]+     |   /* This rule is only useful to print a complete token in error messages */
    /* Unrecognized character */
<*>.    {
    fprintf(stderr, "Unrecognized text on line %d col %d: '%s'.\n",
            yylloc->first_line, yylloc->first_column, yytext);
    return LEX_ERROR;
}

%%
