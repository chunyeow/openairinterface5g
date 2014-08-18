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

#define MIH_C_INTERFACE
#define MIH_C_LOG_C
/*
//-----------------------------------------------------------------------------
#include "MIH_C_log.h"
//-----------------------------------------------------------------------------
#define LOG_BUFF_SIZE  2048
static char        g_log_buffer_time[24];
static char        g_log_buffer[LOG_BUFF_SIZE];
static int         g_bypass_header;
//-----------------------------------------------------------------------------
char* getTimeStamp4Log()
//-----------------------------------------------------------------------------
{
    int len;
    struct timespec time_spec;
    unsigned int time_now_micros;
    unsigned int time_now_s;
    clock_gettime (CLOCK_REALTIME, &time_spec);
    time_now_s      = (unsigned int) time_spec.tv_sec % 3600;
    time_now_micros = (unsigned int) time_spec.tv_nsec/1000;
    //len=snprintf(g_log_buffer_time,24, "%06d:%06d", time_now_s, time_now_micros);
    len=snprintf(g_log_buffer_time,24, "%03d:%03d", time_now_s, time_now_micros);
   return g_log_buffer_time;
}
//-----------------------------------------------------------------------------
int is_newline( char *str, int size){
//-----------------------------------------------------------------------------
    int i;
    for (  i = 0; i < size; i++ ) {
      if ( str[i] == '\n' )
    return 1;
    }
    // if we get all the way to here, there must not have been a newline!
    return 0;
}

// Initialize logging system
int MIH_C_log_init(unsigned int log_outputP) {
//-----------------------------------------------------------------------------
    g_mih_c_log_output = log_outputP;


    g_log_level2string[LOG_EMERG]         = "EMERG";
    g_log_level2string[LOG_ALERT]         = "ALERT";
    g_log_level2string[LOG_CRIT]          = "CRIT ";
    g_log_level2string[LOG_ERR]           = "ERR  ";
    g_log_level2string[LOG_WARNING]       = "WARN ";
    g_log_level2string[LOG_NOTICE]        = "NOTCE";
    g_log_level2string[LOG_INFO]          = "INFO ";
    g_log_level2string[LOG_DEBUG]         = "DEBUG";

    g_bypass_header = 0;

    switch (g_mih_c_log_output){
        case LOG_TO_CONSOLE:
            printf("***** mRAL V%s logging \n\n",MIH_C_VERSION);
            break;
        case LOG_TO_FILE:
            g_mih_c_log_file = fopen(MIH_C_LOGFILE_NAME,"w"); //start over new file
            if (g_mih_c_log_file == NULL){
                perror ("MIH_C_log_init - error opening file");
                exit(1);
            }
            fprintf(g_mih_c_log_file, "***** V%s starting logging \n\n",MIH_C_VERSION);
            fflush(g_mih_c_log_file);
            break;
        case LOG_TO_SYSTEM:
            openlog(MIH_C_SYSLOG_NAME, LOG_PID, LOG_LOCAL7);
            syslog(LOG_NOTICE,  "***** V%s starting logging \n\n",MIH_C_VERSION);
            break;
        default:
            printf("MIH_C_log_init: log_outputP error %d", log_outputP);
    }
    return 0;
}

//-----------------------------------------------------------------------------
// Log messages according to user settings
int MIH_C_log_record(int levelP, const char * log_msgP, ...) {
//-----------------------------------------------------------------------------
    struct timespec time_spec;
    unsigned int    time_now_micros;
    unsigned int    time_now_s;
    va_list         log_ap;
    int             len;


    switch (g_mih_c_log_output){
        case LOG_TO_CONSOLE:
            va_start(log_ap, log_msgP);
            len = vsnprintf(g_log_buffer, LOG_BUFF_SIZE-1, log_msgP, log_ap);
            va_end(log_ap);
            if (g_bypass_header == 0) {
                clock_gettime (CLOCK_REALTIME, &time_spec);
                time_now_s      = (unsigned int) time_spec.tv_sec;
                time_now_micros = (unsigned int) time_spec.tv_nsec/1000;
                printf("[%06d:%06d][%s][%s] ", time_now_s, time_now_micros, MIH_C_SYSLOG_NAME, g_log_level2string[levelP]);
           }
            printf("%s",g_log_buffer);
            fflush(stdout);
            fflush(stderr);
            if (is_newline(g_log_buffer,len) == 0 ){
                g_bypass_header = 1;
            } else {
                g_bypass_header = 0;
            }
            break;
        case LOG_TO_FILE:
            va_start(log_ap, log_msgP);
            len = vsnprintf(g_log_buffer, LOG_BUFF_SIZE-1, log_msgP, log_ap);
            va_end(log_ap);
            if (g_bypass_header == 0) {
                clock_gettime (CLOCK_REALTIME, &time_spec);
                time_now_s      = (unsigned int) time_spec.tv_sec;
                time_now_micros = (unsigned int) time_spec.tv_nsec/1000;
                fprintf(g_mih_c_log_file, "[%06d:%06d][%s][%s] ", time_now_s, time_now_micros, MIH_C_SYSLOG_NAME, g_log_level2string[levelP]);
            }
            fprintf(g_mih_c_log_file, "%s", g_log_buffer);
            fflush(g_mih_c_log_file);
            if (is_newline(g_log_buffer,len) == 0 ){
                g_bypass_header = 1;
            } else {
                g_bypass_header = 0;
            }
            break;
        case LOG_TO_SYSTEM:
            va_start(log_ap, log_msgP);
            syslog(levelP, log_msgP, log_ap);
            va_end(log_ap);
            return 0;
            break;
        default:
            printf("MIH_C_log_record: level error %d", levelP);
  }
  return 0;
}
//-----------------------------------------------------------------------------
// Close logging system
int MIH_C_log_exit(void) {
//-----------------------------------------------------------------------------
    switch (g_mih_c_log_output){
        case LOG_TO_CONSOLE:
            printf("***** stopping logging \n\n");
            break;
        case LOG_TO_FILE:
            fprintf(g_mih_c_log_file, "***** stopping logging \n\n");
            fflush(g_mih_c_log_file);
            fclose(g_mih_c_log_file);
            break;
        case LOG_TO_SYSTEM:
            syslog(LOG_NOTICE,  "***** stopping logging \n\n");
            closelog();
            break;
        default:
            printf("MIH_C_log_exit: output unrecognized error %d", g_mih_c_log_output);
    }
    return 0;
}
*/

