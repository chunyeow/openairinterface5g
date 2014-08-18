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
  
  Address      : Eurecom, Campus SophiaTech, 450 Route des Chappes, CS 50193 - 06904 Biot Sophia Antipolis cedex, FRANCE

 *******************************************************************************/
/*! \file MIH_C_log.h
* \brief This file defines the prototypes of the functions for logging.
* \author BRIZZOLA Davide, GAUTHIER Lionel, MAUREL Frederic, WETTERWALD Michelle
* \date 2012
* \version
* \note
* \bug
* \warning
*/
#error "To be removed"
#ifndef __MIH_C_LOG_H__
#    define __MIH_C_LOG_H__
//-----------------------------------------------------------------------------
#        ifdef MIH_C_LOG_C
#            define private_mih_c_log(x)    x
#            define protected_mih_c_log(x)  x
#            define public_mih_c_log(x)     x
#        else
#            ifdef MIH_C_INTERFACE
#                define private_mih_c_log(x)
#                define protected_mih_c_log(x)  extern x
#                define public_mih_c_log(x)     extern x
#            else
#                define private_mih_c_log(x)
#                define protected_mih_c_log(x)
#                define public_mih_c_log(x)     extern x
#            endif
#        endif
//-----------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <syslog.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
//-----------------------------------------------------------------------------
#define LOG_CONST 0
#define LOG_TO_CONSOLE 0
#define LOG_TO_FILE    1
#define LOG_TO_SYSTEM  2
//default value
#define LOG_TO_DEFAULT LOG_TO_CONSOLE

#define MIH_C_LOGFILE_NAME "/tmp/MIH_C_log.txt"
#define MIH_C_SYSLOG_NAME  "LTE"
#define MIH_C_VERSION     "1.0"  //Release date :  see readme

#define EMERG(args...)   MIH_C_log_record( (LOG_EMERG),   ##args)
#define ALERT(args...)   MIH_C_log_record( (LOG_ALERT),   ##args)
#define CRIT(args...)    MIH_C_log_record( (LOG_CRIT),    ##args)
#define ERR(args...)     MIH_C_log_record( (LOG_ERR),     ##args)
#define WARNING(args...) MIH_C_log_record( (LOG_WARNING), ##args)
#define NOTICE(args...)  MIH_C_log_record( (LOG_NOTICE),  ##args)
#define INFO(args...)    MIH_C_log_record( (LOG_INFO),    ##args)
#define DEBUG(args...)   MIH_C_log_record( (LOG_DEBUG),   ##args)
//-----------------------------------------------------------------------------

private_mih_c_log(FILE *g_mih_c_log_file;)

private_mih_c_log(int g_mih_c_log_output;)

private_mih_c_log(const char* g_log_level2string[LOG_DEBUG+1];)

/*! \fn char* getTimeStamp4Log(void)
* \brief    Get a formated string of current time.
* \return   a char string of current time in seconds and micro seconds.
*/
public_mih_c_log( char* getTimeStamp4Log(void);)


/*! \fn int mih_c_log_init(unsigned int log_outputP)
* \brief    Initialize log module.
* \param[in]  log_outputP       Identifier of output where logs go are sent.
*/
protected_mih_c_log( int MIH_C_log_init(unsigned int log_outputP);)

/*! \fn int MIH_C_log_record(unsigned int levelP, char * log_msgP, ...)
* \brief    Set log parameters.
* \param[in]  levelP                     Level of log (ERROR, DEBUG).
* \param[in]  log_msgP                   Message to log.
*/
public_mih_c_log( int MIH_C_log_record(int level, const char * log_msg, ...);)

/*! \fn int MIH_C_log_exit(void)
* \brief    Close, and clean properly the log module.
*/
protected_mih_c_log( int MIH_C_log_exit(void);)
#endif









