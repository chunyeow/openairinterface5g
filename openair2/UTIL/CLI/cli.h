/*******************************************************************************

  Eurecom OpenAirInterface
  Copyright(c) 1999 - 2011 Eurecom

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
  Forums       : http://forums.eurecom.fsr/openairinterface
  Address      : Eurecom, 2229, route des crêtes, 06560 Valbonne Sophia Antipolis, France

*******************************************************************************/
/*! \file cli.h
* \brief cli interface header file
* \author Navid Nikaein
* \date 2011
* \version 0.1
* \warning This component can be runned only in the user-space
* @ingroup util
*/


#ifndef __CLI_H__
#define __CLI_H__

#include <string.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <malloc.h>
#include <setjmp.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>    

//#include <readline/readline.h>  
//#include <readline/history.h> 

typedef struct {
  
  int port;
  int sfd; //server fd
  int cfd; // client fd
  int enabled;
  int debugfd; // debug state
  int debug_paused;
  char promptchar;
  char prompt[200];
  //	char user_name[200];
  int exit;	
  /*end*/
  
}cli_config;


typedef struct {
  int group;
  char *name;             /* User printable name of the function. */
  void *data;
  int (*func)(char*);
  int (*help)(char*);
  char *doc;			/* Documentation for this function.  */
} command;


typedef void (* cli_handler_t)(const void * data, socklen_t len);


#define MAX_SOCK_BUFFER_SIZE 1500
char buffer[MAX_SOCK_BUFFER_SIZE];

/* The name of this program, as taken from argv[0]. */
char *progname;
/*username*/
const char *username;
const char *host;
/* When non-zero, this global means the user is done using this program. */
int done;



/// global permissions table
unsigned int map_permissions;

sigjmp_buf openair_jmp_buf;

/// maximum session ID length
#define MAX_SID 100
/// session id - sent in every message
char g_sid[MAX_SID];

#define WELCOME_MSG "\n\nWelcome to the Eurecom OpenAirInterface CLI \n\n"

#define NOCOMMAND_MSG "No such command for openair CLI Interface"


#define SHOW_PERMISSION     (1 << 0)

#define CONFIG_PERMISSION   (1 << 1)

#define ADMIN_PERMISSION    (1 << 2)

#define OK 0
#define ERR -1

#define CLI_MAX_CMDS 200
#define CLI_MAX_NODES 10

/* The names of functions that actually do the manipulation. */
int com_help (), com_exit ();

int prompt(),prompt_usage(), info();

int start(), set();
int start_usage(), set_usage();

/* Forward declarations. */
char *stripwhite ();
command *find_command ();
void abandon_input(int);
char *command_generator ();
char **fileman_completion ();
int cli_login(const char *, int, int );
int cli_loop(char* msg);
int cli_set_prompt_char(void);
char *cli_prompt(void);
int openair_cli(void);
int valid_argument (char *caller, char *arg);
int cli_help(char *caller, char * arg);
void cli_init(void);
int cli_start(void);
void set_comp_debug(int);
int set_permissions_map(void);
int is_debugging(void);
int execute_line(char* line);
int whitespace (char  c);
void cli_finish(void);
int token_argument(char *arg, char* optv[]);
int process_argument(int optc, char* optv[]);
#include "cli_if.h"

#endif
