/*******************************************************************************

  Eurecom OpenAirInterface
  Copyright(c) 1999 - 2013 Eurecom

  This program is free software; you can redistribute it and/or modify it
  under the terms and conditions of the GNU General Public License,
  version 2, as published by the Free Software Foundation.

  This program is distributed in the hope it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
  more details.

  You should have received a copy of the GNU General Public License along with
  this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.

  The full GNU General Public License is included in this distribution in
  the file called "COPYING".

  Contact Information
  Openair Admin: openair_admin@eurecom.fr
  Openair Tech : openair_tech@eurecom.fr
  Forums       : http://forums.eurecom.fr/openairinterface
  Address      : EURECOM, Campus SophiaTech, 450 Route des Chappes
                 06410 Biot FRANCE

*******************************************************************************/

/* For development only : */
%debug
%error-verbose

%parse-param {struct hss_config_s *hss_config_p}

/* Keep track of location */
%locations
%defines
%pure-parser

%{
#include <stdio.h>
#include <errno.h>

#include "hss_config.h"
#include "hss_parser.h"

void yyerror (YYLTYPE *llocp, struct hss_config_s *hss_config_p, const char *s);

extern int yywrap();
extern int yylex();

/* The Lex parser prototype */
int fddlex(YYSTYPE *lvalp, YYLTYPE *llocp);
%}

/* Values returned by lex for token */
%union {
    char         *string;   /* The string is allocated by strdup in lex.*/
    int           integer;  /* Store integer values */
}

%token <string> QSTRING
%token <integer> INTEGER

%token LEX_ERROR
%token FDCONF
%token MYSQL_USER
%token MYSQL_SERVER
%token MYSQL_PASS
%token MYSQL_DB

%%
conffile:       /* Empty is OK -- for simplicity here, we reject in daemon later */
    | conffile mysql_db
    | conffile mysql_server
    | conffile mysql_user
    | conffile mysql_pass
    | conffile fdconf
    | conffile errors
    {
        yyerror(&yylloc, hss_config_p, "An error occurred while parsing the configuration file");
        return EINVAL;
    }
    ;

mysql_db: MYSQL_DB '=' QSTRING ';'
    {
        hss_config_p->mysql_database = $3;
    }
    ;

mysql_pass: MYSQL_PASS '=' QSTRING ';'
    {
        hss_config_p->mysql_password = $3;
    }
    ;

mysql_user: MYSQL_USER '=' QSTRING ';'
    {
        hss_config_p->mysql_user = $3;
    }
    ;

mysql_server: MYSQL_SERVER '=' QSTRING ';'
    {
        hss_config_p->mysql_server = $3;
    }
    ;

fdconf: FDCONF '=' QSTRING ';'
    {
        hss_config_p->freediameter_config = $3;
    }
    ;

    /* Lexical or syntax error */
errors: LEX_ERROR
    | error
    ;
%%

void yyerror(YYLTYPE *llocp, struct hss_config_s *hss_config_p, const char *str)
{
    fprintf(stderr, "Error in %s ( on line %i column %i -> line %i column %i) : %s\n",
            hss_config_p->freediameter_config, llocp->first_line, llocp->first_column,
            llocp->last_line, llocp->last_column, str);
}
