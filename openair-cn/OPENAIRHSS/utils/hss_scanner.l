/*******************************************************************************
    OpenAirInterface
    Copyright(c) 1999 - 2014 Eurecom

    OpenAirInterface is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.


    OpenAirInterface is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with OpenAirInterface.The full GNU General Public License is
   included in this distribution in the file called "COPYING". If not,
   see <http://www.gnu.org/licenses/>.

  Contact Information
  OpenAirInterface Admin: openair_admin@eurecom.fr
  OpenAirInterface Tech : openair_tech@eurecom.fr
  OpenAirInterface Dev  : openair4g-devel@eurecom.fr

  Address      : Eurecom, Compus SophiaTech 450, route des chappes, 06451 Biot, France.

 *******************************************************************************/

/* Lex configuration parser.
 *
 * This file defines the token for parsing the configuration file
 *
 * Note : This module is NOT thread-safe. All processing must be done from one thread only.
 */
%{
#include <stdio.h>

#include "hss_config.h"
/* Include yacc tokens definitions */
#include "hss_parser.h"

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

%option bison-bridge
%option bison-locations
%option noyywrap
%option nounput

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

    /* Full words tokens (keywords) */
(?i:"FD_conf")      { return FDCONF; }
(?i:"MYSQL_server") { return MYSQL_SERVER; }
(?i:"MYSQL_user")   { return MYSQL_USER; }
(?i:"MYSQL_pass")   { return MYSQL_PASS; }
(?i:"MYSQL_db")     { return MYSQL_DB; }

    /* Valid single characters for yyparse */
<*>[=,:;{}]     { return yytext[0]; }

<*>[[:alnum:]]+     |   /* This rule is only useful to print a complete token in error messages */
    /* Unrecognized character */
<*>.    {
    fprintf(stderr, "Unrecognized text on line %d col %d: '%s'.\n",
            yylloc->first_line, yylloc->first_column, yytext);
    return LEX_ERROR;
}

%%
