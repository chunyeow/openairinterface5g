/*******************************************************************************

  Eurecom OpenAirInterface
  Copyright(c) 1999 - 2010 Eurecom

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
/*! \file log.c
* \brief log implementaion
* \author Navid Nikaein
* \date 2011
* \version 0.5
* @ingroup util

*/

//#define LOG_TEST 1

#define COMPONENT_LOG
#define COMPONENT_LOG_IF



//static unsigned char       fifo_print_buffer[FIFO_PRINTF_MAX_STRING_SIZE];

#include "log.h"
#include "log_vars.h"
#include "vcd_signal_dumper.h"

//#include "UTIL/OCG/OCG.h"
//#include "UTIL/OCG/OCG_extern.h"
#ifdef USER_MODE
#include <string.h>
#endif
#ifdef RTAI
#include <rtai.h>
#include <rtai_fifos.h>
#    define FIFO_PRINTF_MAX_STRING_SIZE   1000
#    define FIFO_PRINTF_NO              62
#    define FIFO_PRINTF_SIZE            65536
#endif

// made static and not local to logRecord() for performance reasons
static char g_buff_tmp  [MAX_LOG_ITEM];
static char g_buff_info [MAX_LOG_INFO];
static char g_buff_total[MAX_LOG_TOTAL];

static int gfd;

static char *log_level_highlight_start[] = {LOG_RED, LOG_RED, LOG_RED, LOG_RED, LOG_ORANGE, LOG_BLUE, "", ""};	/*!< \brief Optional start-format strings for highlighting */
static char *log_level_highlight_end[]   = {LOG_RESET, LOG_RESET, LOG_RESET, LOG_RESET, LOG_RESET,LOG_RESET,  "",""};	/*!< \brief Optional end-format strings for highlighting */


static int bypass_log_hdr;

//extern MAC_xface *mac_xface;

int logInit (void) {
  
#ifdef USER_MODE
  int i;
  g_log = calloc(1, sizeof(log_t));
  memset(g_log, 0, sizeof(log_t));
#else
  g_log = kmalloc(sizeof(log_t),GFP_KERNEL);
#endif
  if (g_log == NULL) {
#ifdef USER_MODE
    perror ("cannot allocated memory for log generation modeul \n");
    exit(-1);
#else
    printk("cannot allocated memory for log generation modeul \n");
    return(-1);
#endif
  }
  
  g_log->log_component[PHY].name = "PHY";
    g_log->log_component[PHY].level = LOG_EMERG;
    g_log->log_component[PHY].flag =  LOG_MED;
    g_log->log_component[PHY].interval =  1;
    g_log->log_component[PHY].fd = 0;
    g_log->log_component[PHY].filelog = 0;
    g_log->log_component[PHY].filelog_name = "/tmp/phy.log";
    
    g_log->log_component[MAC].name = "MAC";
    g_log->log_component[MAC].level = LOG_EMERG;
    g_log->log_component[MAC].flag =  LOG_MED;
    g_log->log_component[MAC].interval =  1;
    g_log->log_component[MAC].fd = 0;
    g_log->log_component[MAC].filelog = 0;
    g_log->log_component[MAC].filelog_name = "/tmp/mac.log";
    
    g_log->log_component[OPT].name = "OPT";
    g_log->log_component[OPT].level = LOG_EMERG;
    g_log->log_component[OPT].flag = LOG_MED;
    g_log->log_component[OPT].interval =  1;
    g_log->log_component[OPT].fd = 0;
    g_log->log_component[OPT].filelog = 0;
    g_log->log_component[OPT].filelog_name = "";

    g_log->log_component[RLC].name = "RLC";
    g_log->log_component[RLC].level = LOG_INFO;
    g_log->log_component[RLC].flag = LOG_MED;
    g_log->log_component[RLC].interval =  1;
    g_log->log_component[RLC].fd = 0;
    g_log->log_component[RLC].filelog = 0;
    g_log->log_component[RLC].filelog_name = "/tmp/rlc.log";
    
    g_log->log_component[PDCP].name = "PDCP";
    g_log->log_component[PDCP].level = LOG_INFO;
    g_log->log_component[PDCP].flag = LOG_MED;
    g_log->log_component[PDCP].interval =  1;
    g_log->log_component[PDCP].fd = 0;
    g_log->log_component[PDCP].filelog = 0;
    g_log->log_component[PDCP].filelog_name = "/tmp/pdcp.log";
    
    g_log->log_component[RRC].name = "RRC";
    g_log->log_component[RRC].level = LOG_TRACE;
    g_log->log_component[RRC].flag = LOG_MED;
    g_log->log_component[RRC].interval =  1;
    g_log->log_component[RRC].fd = 0;
    g_log->log_component[RRC].filelog = 0;
    g_log->log_component[RRC].filelog_name = "/tmp/rrc.log";

    g_log->log_component[EMU].name = "EMU";
    g_log->log_component[EMU].level = LOG_EMERG;
    g_log->log_component[EMU].flag =  LOG_MED; 
    g_log->log_component[EMU].interval =  1;
    g_log->log_component[EMU].fd = 0;
    g_log->log_component[EMU].filelog = 0;
    g_log->log_component[EMU].filelog_name = "";
    
    g_log->log_component[OMG].name = "OMG";
    g_log->log_component[OMG].level = LOG_EMERG;
    g_log->log_component[OMG].flag =  LOG_MED;
    g_log->log_component[OMG].interval =  1;
    g_log->log_component[OMG].fd = 0;
    g_log->log_component[OMG].filelog = 0;
    g_log->log_component[OMG].filelog_name = "";
    
    g_log->log_component[OTG].name = "OTG";
    g_log->log_component[OTG].level = LOG_EMERG;
    g_log->log_component[OTG].flag =  LOG_MED;
    g_log->log_component[OTG].interval =  1;
    g_log->log_component[OTG].fd = 0;
    g_log->log_component[OTG].filelog = 0;
    g_log->log_component[OTG].filelog_name = "/tmp/otg.log";

    g_log->log_component[OTG_LATENCY].name = "OTG_LATENCY";
    g_log->log_component[OTG_LATENCY].level = LOG_EMERG;
    g_log->log_component[OTG_LATENCY].flag =  LOG_MED;
    g_log->log_component[OTG_LATENCY].interval =  1;
    g_log->log_component[OTG_LATENCY].fd = 0;
    g_log->log_component[OTG_LATENCY].filelog = 0;
    g_log->log_component[OTG_LATENCY].filelog_name = "/tmp/otg_latency.dat";

    g_log->log_component[OTG_LATENCY_BG].name = "OTG_LATENCY_BG";
    g_log->log_component[OTG_LATENCY_BG].level = LOG_EMERG;
    g_log->log_component[OTG_LATENCY_BG].flag =  LOG_MED;
    g_log->log_component[OTG_LATENCY_BG].interval =  1;
    g_log->log_component[OTG_LATENCY_BG].fd = 0;
    g_log->log_component[OTG_LATENCY_BG].filelog = 0;
    g_log->log_component[OTG_LATENCY_BG].filelog_name = "/tmp/otg_latency_bg.dat";


    g_log->log_component[OTG_GP].name = "OTG_GP";
    g_log->log_component[OTG_GP].level = LOG_EMERG;
    g_log->log_component[OTG_GP].flag =  LOG_MED;
    g_log->log_component[OTG_GP].interval =  1;
    g_log->log_component[OTG_GP].fd = 0;
    g_log->log_component[OTG_GP].filelog = 0;
    g_log->log_component[OTG_GP].filelog_name = "/tmp/otg_GP.dat";

    g_log->log_component[OTG_GP_BG].name = "OTG_GP_BG";
    g_log->log_component[OTG_GP_BG].level = LOG_EMERG;
    g_log->log_component[OTG_GP_BG].flag =  LOG_MED;
    g_log->log_component[OTG_GP_BG].interval =  1;
    g_log->log_component[OTG_GP_BG].fd = 0;
    g_log->log_component[OTG_GP_BG].filelog = 0;
    g_log->log_component[OTG_GP_BG].filelog_name = "/tmp/otg_GP_bg.dat";

    g_log->log_component[OTG_JITTER].name = "OTG_JITTER";
    g_log->log_component[OTG_JITTER].level = LOG_EMERG;
    g_log->log_component[OTG_JITTER].flag =  LOG_MED;
    g_log->log_component[OTG_JITTER].interval =  1;
    g_log->log_component[OTG_JITTER].fd = 0;
    g_log->log_component[OTG_JITTER].filelog = 0;
    g_log->log_component[OTG_JITTER].filelog_name = "/tmp/otg_jitter.dat";
    
    g_log->log_component[OCG].name = "OCG";
    g_log->log_component[OCG].level = LOG_EMERG;
    g_log->log_component[OCG].flag =  LOG_MED;
    g_log->log_component[OCG].interval =  1;
    g_log->log_component[OCG].fd = 0;
    g_log->log_component[OCG].filelog = 0;
    g_log->log_component[OCG].filelog_name = "";
    
    g_log->log_component[PERF].name = "PERF";
    g_log->log_component[PERF].level = LOG_EMERG;
    g_log->log_component[PERF].flag =  LOG_MED;
    g_log->log_component[PERF].interval =  1;
    g_log->log_component[PERF].fd = 0;
    g_log->log_component[PERF].filelog = 0;
    g_log->log_component[PERF].filelog_name = "";
    
    g_log->log_component[OIP].name = "OIP";
    g_log->log_component[OIP].level = LOG_EMERG;
    g_log->log_component[OIP].flag =  LOG_MED;
    g_log->log_component[OIP].interval =  1;
    g_log->log_component[OIP].fd = 0;
    g_log->log_component[OIP].filelog = 0;
    g_log->log_component[OIP].filelog_name = "";
    
    g_log->log_component[CLI].name = "CLI";
    g_log->log_component[CLI].level = LOG_EMERG;
    g_log->log_component[CLI].flag =  LOG_MED;
    g_log->log_component[CLI].interval =  1;
    g_log->log_component[CLI].fd = 0;
    g_log->log_component[CLI].filelog =  0;
    g_log->log_component[CLI].filelog_name = "";
     
    g_log->log_component[MSC].name = "MSC";
    g_log->log_component[MSC].level = LOG_EMERG;
    g_log->log_component[MSC].flag =  LOG_MED;
    g_log->log_component[MSC].interval =  1;
    g_log->log_component[MSC].fd = 0;
    g_log->log_component[MSC].filelog =  0;
    g_log->log_component[MSC].filelog_name = "/tmp/msc.log";
 
    g_log->log_component[OCM].name = "OCM";
    g_log->log_component[OCM].level = LOG_EMERG;
    g_log->log_component[OCM].flag =  LOG_MED;
    g_log->log_component[OCM].interval =  1;
    g_log->log_component[OCM].fd = 0;
    g_log->log_component[OCM].filelog =  0;
    g_log->log_component[OCM].filelog_name = "/tmp/ocm.log";

    g_log->log_component[S1AP].name = "S1AP";
    g_log->log_component[S1AP].level = LOG_EMERG;
    g_log->log_component[S1AP].flag = LOG_MED;
    g_log->log_component[S1AP].interval = 1;
    g_log->log_component[S1AP].fd = 0;
    g_log->log_component[S1AP].filelog = 0;
    g_log->log_component[S1AP].filelog_name = "/tmp/s1ap.log";

    g_log->log_component[SCTP].name = "SCTP";
    g_log->log_component[SCTP].level = LOG_EMERG;
    g_log->log_component[SCTP].flag = LOG_MED;
    g_log->log_component[SCTP].interval = 1;
    g_log->log_component[SCTP].fd = 0;
    g_log->log_component[SCTP].filelog = 0;
    g_log->log_component[SCTP].filelog_name = "";

    g_log->log_component[HW].name = "HW";
    g_log->log_component[HW].level = LOG_EMERG;
    g_log->log_component[HW].flag = LOG_MED;
    g_log->log_component[HW].interval = 1;
    g_log->log_component[HW].fd = 0;
    g_log->log_component[HW].filelog = 0;
    g_log->log_component[HW].filelog_name = "";

    g_log->level2string[LOG_EMERG]         = "G"; //EMERG
    g_log->level2string[LOG_ALERT]         = "A"; // ALERT
    g_log->level2string[LOG_CRIT]          = "C"; // CRITIC
    g_log->level2string[LOG_ERR]           = "E"; // ERROR
    g_log->level2string[LOG_WARNING]       = "W"; // WARNING
    g_log->level2string[LOG_NOTICE]        = "N"; // NOTICE
    g_log->level2string[LOG_INFO]          = "I"; //INFO
    g_log->level2string[LOG_DEBUG]         = "D"; // DEBUG
    g_log->level2string[LOG_FILE]          = "F"; // file
    g_log->level2string[LOG_TRACE]         = "T"; // TRACE
    
    g_log->onlinelog = 1; //online log file
    g_log->syslog = 0; 
    g_log->filelog   = 0;
    g_log->level  = LOG_TRACE;
    g_log->flag   = LOG_LOW;
    
#ifndef RTAI
  g_log->config.remote_ip      = 0;
  g_log->config.remote_level   = LOG_EMERG;
  g_log->config.facility       = LOG_LOCAL7;
  g_log->config.audit_ip       = 0;
  g_log->config.audit_facility = LOG_LOCAL6;
  g_log->config.format         = 0x00; // online debug inactive
  
  g_log->filelog_name = "/tmp/openair.log";
 
  if (g_log->syslog) {
    openlog(g_log->log_component[EMU].name, LOG_PID, g_log->config.facility);
  } 
  if (g_log->filelog) {
    gfd = open(g_log->filelog_name, O_WRONLY | O_CREAT, 0666);
  }
  // could put a loop here to check for all comps
  for (i=MIN_LOG_COMPONENTS; i < MAX_LOG_COMPONENTS; i++){
    if (g_log->log_component[i].filelog == 1 ) 
      g_log->log_component[i].fd = open(g_log->log_component[i].filelog_name, O_WRONLY | O_CREAT | O_APPEND, 0666);
  }
#else
  g_log->syslog = 0; 
  g_log->filelog   = 0;
  rtf_create (FIFO_PRINTF_NO, FIFO_PRINTF_SIZE);
#endif

#ifdef USER_MODE  
  printf("log init done\n");
#else
  printk("log init done\n");
#endif

}

//log record: add to a list 
void logRecord( const char *file, const char *func,
		int line,  int comp, int level, 
		char *format, ...) {

  va_list args;
  LOG_params log_params;
  int len, err;
  //LOG_elt *log = NULL;

  va_start(args, format);
  len = vsnprintf(log_params.l_buff_info, MAX_LOG_INFO-1, format, args);
  va_end(args);

  //2 first parameters must be passed as 'const' to the thread function
  log_params.file = file;
  log_params.func = func;
  log_params.line = line;
  log_params.comp = comp;
  log_params.level = level;
  log_params.format = format;
  log_params.len = len;

  if(pthread_mutex_lock(&log_lock) != 0) { return; }

  log_list_tail++;
  log_list[log_list_tail - 1] = log_params;
  if (log_list_tail >= 1000) log_list_tail = 0;
  if (log_list_nb_elements < 1000) log_list_nb_elements++;
  if(pthread_cond_signal(&log_notify) != 0) { return; }

  if(pthread_mutex_unlock(&log_lock) != 0) { return; }

  //log = malloc(sizeof(LOG_elt));
  //log->next = NULL;
  //log->log_params = log_params;
  /* Add log task to queue */
  //log_list_add_tail_eurecom(log, &log_list);

}

void logRecord_thread_safe(const char *file, const char *func,
                           int line,  int comp, int level,
                           int len, const char *params_string) {

  log_component_t *c;

  sprintf(g_buff_info, "%s", params_string);

  vcd_signal_dumper_dump_function_by_name(VCD_SIGNAL_DUMPER_FUNCTIONS_LOG_RECORD,1);

  g_buff_total[0] = '\0';
  c = &g_log->log_component[comp];

  // do not apply filtering for LOG_F
  // only log messages which are enabled and are below the global log level and component's level threshold
  if ( (level != LOG_FILE) && ( (c->level > g_log->level) || (level > c->level) || (level > g_log->level)) ){
    //  || ((mac_xface->frame % c->interval) != 0)) {
    vcd_signal_dumper_dump_function_by_name(VCD_SIGNAL_DUMPER_FUNCTIONS_LOG_RECORD,0);
    return;
  }

  // adjust syslog level for TRACE messages
  if (g_log->syslog) {
    if (g_log->level > LOG_DEBUG) {
      g_log->level = LOG_DEBUG;
    }
  }

  //if (level == LOG_TRACE)
  //exit(-1);
  //printf (g_buff_info);
  //return;
  // make sure that for log trace the extra info is only printed once, reset when the level changes
  if ((level == LOG_FILE) ||  (c->flag == LOG_NONE)  || (level ==LOG_TRACE )){
    strncat(g_buff_total, g_buff_info, MAX_LOG_TOTAL-1);
  }
  else{
    if ( (g_log->flag & FLAG_COLOR) || (c->flag & FLAG_COLOR) )  {
      len+=snprintf(g_buff_tmp, MAX_LOG_ITEM, "%s",
                    log_level_highlight_start[level]);
      strncat(g_buff_total, g_buff_tmp, MAX_LOG_TOTAL-1);
    }

    if ( (g_log->flag & FLAG_COMP) || (c->flag & FLAG_COMP) ){
      len+=snprintf(g_buff_tmp, MAX_LOG_ITEM, "[%s]",
                    g_log->log_component[comp].name);
      strncat(g_buff_total, g_buff_tmp, MAX_LOG_TOTAL-1);
    }

    if ( (g_log->flag & FLAG_LEVEL) || (c->flag & FLAG_LEVEL) ){
      len+=snprintf(g_buff_tmp, MAX_LOG_ITEM, "[%s]",
                    g_log->level2string[level]);
      strncat(g_buff_total, g_buff_tmp, MAX_LOG_TOTAL-1);
    }

    if (  (g_log->flag & FLAG_FUNCT) || (c->flag & FLAG_FUNCT) )  {
      len+=snprintf(g_buff_tmp, MAX_LOG_ITEM, "[%s] ",
                    func);
      strncat(g_buff_total, g_buff_tmp, MAX_LOG_TOTAL-1);
    }

    if (  (g_log->flag & FLAG_FILE_LINE) || (c->flag & FLAG_FILE_LINE) )  {
      len+=snprintf(g_buff_tmp, MAX_LOG_ITEM, "[%s:%d]",
                    file,line);
      strncat(g_buff_total, g_buff_tmp, MAX_LOG_TOTAL-1);
    }
    strncat(g_buff_total, g_buff_info, MAX_LOG_TOTAL-1);


    if (  (g_log->flag & FLAG_COLOR) || (c->flag & FLAG_COLOR) )  {
      len+=snprintf(g_buff_tmp, MAX_LOG_ITEM, "%s",
                    log_level_highlight_end[level]);
      strncat(g_buff_total, g_buff_tmp, MAX_LOG_TOTAL-1);
    }

    /* // log trace and not reach a new line with 3 bytes
if ((level == LOG_TRACE) && (is_newline(g_buff_info,3) == 0 )){
    bypass_log_hdr = 1;
}
else
  bypass_log_hdr = 0;
*/



    //  strncat(g_buff_total, "\n", MAX_LOG_TOTAL);
  }

  // OAI printf compatibility
  if ((g_log->onlinelog == 1) && (level != LOG_FILE)) {
#ifdef RTAI
    if (len > MAX_LOG_TOTAL) {
      rt_printk ("[OPENAIR] FIFO_PRINTF WROTE OUTSIDE ITS MEMORY BOUNDARY : ERRORS WILL OCCUR\n");
    }
    if (len > 0) {
      rtf_put (FIFO_PRINTF_NO, g_buff_total, len);
    }
#else
    printf("%s",g_buff_total);
#endif
  }

#ifndef RTAI
  if (g_log->syslog) {
    syslog(g_log->level, g_buff_total);
  }
  if (g_log->filelog) {
    write(gfd, g_buff_total, strlen(g_buff_total));
  }
  if ((g_log->log_component[comp].filelog) && (level == LOG_FILE)) {
    write(g_log->log_component[comp].fd, g_buff_total, strlen(g_buff_total));
  }
#endif

  vcd_signal_dumper_dump_function_by_name(VCD_SIGNAL_DUMPER_FUNCTIONS_LOG_RECORD,0);

}

void *log_thread_function(void * list) {

  LOG_params log_params;

  for(;;) {

    //log_elt = NULL;

    /* Lock must be taken to wait on conditional variable */
    pthread_mutex_lock(&log_lock);

    /* Wait on condition variable, check for spurious wakeups.
       When returning from pthread_cond_wait(), we own the lock. */

    // sleep
    while((log_list_nb_elements == 0) && (log_shutdown == 0)) {
      pthread_cond_wait(&log_notify, &log_lock);
    }
    // exit 
    if((log_shutdown==1) && (log_list_nb_elements == 0)) {
      break;
    }

    /* Grab our task */
    //log_elt = log_list_remove_head(&log_list);
    log_params = log_list[log_list_head];
    log_list_head++;
    log_list_nb_elements--;
    if (log_list_head >= 1000) log_list_head = 0;


    /* Unlock */
    pthread_mutex_unlock(&log_lock);

    /* Get to work */
    logRecord_thread_safe(log_params.file, 
			  log_params.func, 
			  log_params.line, 
			  log_params.comp, 
			  log_params.level, 
			  log_params.len, 
			  log_params.l_buff_info);

    //free(log_elt);
  }
}


//log record, format, and print:  executed in the main thread (mt)
void logRecord_mt( const char *file, const char *func,
		int line,  int comp, int level, 
		char *format, ...) {
   
  int len, i;
  va_list args;
  log_component_t *c;

  g_buff_total[0] = '\0';
  c = &g_log->log_component[comp];
  
  // do not apply filtering for LOG_F
  // only log messages which are enabled and are below the global log level and component's level threshold
  if ( (level != LOG_FILE) && ( (c->level > g_log->level) || (level > c->level) || (level > g_log->level)) ){
    //  || ((mac_xface->frame % c->interval) != 0)) { 
    return;
   }
  vcd_signal_dumper_dump_function_by_name(VCD_SIGNAL_DUMPER_FUNCTIONS_LOG_RECORD,1);
   // adjust syslog level for TRACE messages
   if (g_log->syslog) {
     if (g_log->level > LOG_DEBUG) { 
       g_log->level = LOG_DEBUG;
     }
   }
  va_start(args, format);
  len=vsnprintf(g_buff_info, MAX_LOG_INFO-1, format, args);
  va_end(args);


  //if (level == LOG_TRACE)
  //exit(-1);
  //printf (g_buff_info);
  //return; 
 // make sure that for log trace the extra info is only printed once, reset when the level changes
  if ((level == LOG_FILE) ||  (c->flag == LOG_NONE)  || (level ==LOG_TRACE )){
    strncat(g_buff_total, g_buff_info, MAX_LOG_TOTAL-1);
  } 
  else{
    if ( (g_log->flag & FLAG_COLOR) || (c->flag & FLAG_COLOR) )  {
      len+=snprintf(g_buff_tmp, MAX_LOG_ITEM, "%s",
		    log_level_highlight_start[level]);
      strncat(g_buff_total, g_buff_tmp, MAX_LOG_TOTAL-1);
    }
    
    if ( (g_log->flag & FLAG_COMP) || (c->flag & FLAG_COMP) ){
      len+=snprintf(g_buff_tmp, MAX_LOG_ITEM, "[%s]",
		    g_log->log_component[comp].name);
      strncat(g_buff_total, g_buff_tmp, MAX_LOG_TOTAL-1);
    }
  
    if ( (g_log->flag & FLAG_LEVEL) || (c->flag & FLAG_LEVEL) ){
      len+=snprintf(g_buff_tmp, MAX_LOG_ITEM, "[%s]",
		    g_log->level2string[level]);
      strncat(g_buff_total, g_buff_tmp, MAX_LOG_TOTAL-1);
    }
    
    if (  (g_log->flag & FLAG_FUNCT) || (c->flag & FLAG_FUNCT) )  {
      len+=snprintf(g_buff_tmp, MAX_LOG_ITEM, "[%s] ",
		    func);
      strncat(g_buff_total, g_buff_tmp, MAX_LOG_TOTAL-1);
    }
    
    if (  (g_log->flag & FLAG_FILE_LINE) || (c->flag & FLAG_FILE_LINE) )  {
      len+=snprintf(g_buff_tmp, MAX_LOG_ITEM, "[%s:%d]",
		    file,line);
      strncat(g_buff_total, g_buff_tmp, MAX_LOG_TOTAL-1);
    }
    strncat(g_buff_total, g_buff_info, MAX_LOG_TOTAL-1);
    
   
    if (  (g_log->flag & FLAG_COLOR) || (c->flag & FLAG_COLOR) )  {
      len+=snprintf(g_buff_tmp, MAX_LOG_ITEM, "%s", 
		    log_level_highlight_end[level]);
      strncat(g_buff_total, g_buff_tmp, MAX_LOG_TOTAL-1);
    }

  /* // log trace and not reach a new line with 3 bytes
  if ((level == LOG_TRACE) && (is_newline(g_buff_info,3) == 0 )){
      bypass_log_hdr = 1;
  }
  else
    bypass_log_hdr = 0;
  */
  

 
  //  strncat(g_buff_total, "\n", MAX_LOG_TOTAL);
  }

  // OAI printf compatibility 
  if ((g_log->onlinelog == 1) && (level != LOG_FILE)) 
#ifdef RTAI
  if (len > MAX_LOG_TOTAL) {
    rt_printk ("[OPENAIR] FIFO_PRINTF WROTE OUTSIDE ITS MEMORY BOUNDARY : ERRORS WILL OCCUR\n");
  }
  if (len > 0) {
   rtf_put (FIFO_PRINTF_NO, g_buff_total, len);
  }
#else
    printf("%s",g_buff_total);
#endif

#ifndef RTAI
  if (g_log->syslog) {
    syslog(g_log->level, g_buff_total);
  } 
  if (g_log->filelog) {
    write(gfd, g_buff_total, strlen(g_buff_total));
  } 
  if ((g_log->log_component[comp].filelog) && (level == LOG_FILE)) {
      write(g_log->log_component[comp].fd, g_buff_total, strlen(g_buff_total));
  }
#endif

  vcd_signal_dumper_dump_function_by_name(VCD_SIGNAL_DUMPER_FUNCTIONS_LOG_RECORD,0);

}
int  set_log(int component, int level, int interval) {
  
  if ((component >=MIN_LOG_COMPONENTS) && (component < MAX_LOG_COMPONENTS)){
    if ((level <= LOG_TRACE) && (level >= LOG_EMERG)){
      g_log->log_component[component].level = level;
      switch (level) {
      case LOG_TRACE: 
	g_log->log_component[component].flag = LOG_MED ;
	break;
      case LOG_DEBUG:
	g_log->log_component[component].flag = LOG_MED ;
	break;
      case LOG_INFO:
	g_log->log_component[component].flag = LOG_LOW ;
	break;
      default:
	g_log->log_component[component].flag = LOG_NONE ;
	break;
      }
    }
    if ((interval > 0) && (interval <= 0xFF)){
      g_log->log_component[component].interval = interval;
    }
    return 0;
  }
  else
    return -1;
}

int  set_comp_log(int component, int level, int verbosity, int interval) {
  
  if ((component >=MIN_LOG_COMPONENTS) && (component < MAX_LOG_COMPONENTS)){
    if ((verbosity == LOG_NONE) || (verbosity == LOG_LOW) || (verbosity == LOG_MED) || (verbosity == LOG_FULL) || (verbosity == LOG_HIGH) ) {
      g_log->log_component[component].flag = verbosity; 
    }
    if ((level <= LOG_TRACE) && (level >= LOG_EMERG)){
	 g_log->log_component[component].level = level;
    }
    if ((interval > 0) && (interval <= 0xFF)){
      g_log->log_component[component].interval = interval;
    }
    return 0;
  }
  else
    return -1;
}

void set_glog(int level, int verbosity) {
  g_log->level = level;
  g_log->flag = verbosity;
}
void set_glog_syslog(int enable) {
  g_log->syslog = enable;
}
void set_glog_onlinelog(int enable) {
  g_log->onlinelog = enable;
}
void set_glog_filelog(int enable) {
  g_log->filelog = enable;
}

void set_component_filelog(int comp){
  
  if (g_log->log_component[comp].filelog ==  0){
    g_log->log_component[comp].filelog =  1;
    if (g_log->log_component[comp].fd == 0)
      g_log->log_component[comp].fd = open(g_log->log_component[comp].filelog_name, O_WRONLY | O_CREAT | O_TRUNC, 0666);
  }
}

/*
 * for the two functions below, the passed array must have a final entry
 * with string value NULL
 */
/* map a string to an int. Takes a mapping array and a string as arg */
int map_str_to_int(mapping *map, const char *str){
    while (1) {
        if (map->name == NULL) {
            return(-1);
        }
        if (!strcmp(map->name, str)) {
            return(map->value);
        }
        map++;
    }
}

/* map an int to a string. Takes a mapping array and a value */
char *map_int_to_str(mapping *map, int val) {
    while (1) {
        if (map->name == NULL) {
            return NULL;
        }
        if (map->value == val) {
            return map->name;
        }
        map++;
    }
}
int is_newline( char *str, int size){
    int i;
    for (  i = 0; i < size; i++ ) {
      if ( str[i] == '\n' )
	return 1;
    }
    /* if we get all the way to here, there must not have been a newline! */
    return 0;
}
void logClean (void) {
  int i;
#ifdef RTAI
  rtf_destroy (FIFO_PRINTF_NO);
#else
  if (g_log->syslog) {
    closelog();
  } 
  if (g_log->filelog) {
    close(gfd);
  }
  for (i=MIN_LOG_COMPONENTS; i < MAX_LOG_COMPONENTS; i++){
    if (g_log->log_component[i].filelog) 
      close(g_log->log_component[i].fd);
  }
#endif

}


#ifdef LOG_TEST

int
main(int argc, char *argv[]) {

  logInit();

  //set_log_syslog(1);
  test_log();
  
  return 1;
}

int test_log(){

  LOG_ENTER(MAC); // because the default level is DEBUG
  LOG_I(EMU, "1 Starting OAI logs version %s Build date: %s on %s\n", 
	       BUILD_VERSION, BUILD_DATE, BUILD_HOST);  
  LOG_D(MAC, "1 debug  MAC \n");
  LOG_N(MAC, "1 notice MAC \n");
  LOG_W(MAC, "1 warning MAC \n");
 
  set_comp_log(EMU, LOG_INFO, FLAG_ONLINE);
  set_comp_log(MAC, LOG_WARNING, 0);
  
  LOG_I(EMU, "2 Starting OAI logs version %s Build date: %s on %s\n", 
	       BUILD_VERSION, BUILD_DATE, BUILD_HOST);  
  LOG_E(MAC, "2 emerge MAC\n");
  LOG_D(MAC, "2 debug  MAC \n");
  LOG_N(MAC, "2 notice MAC \n");
  LOG_W(MAC, "2 warning MAC \n");
  LOG_I(MAC, "2 info MAC \n");
  
  
  set_comp_log(MAC, LOG_NOTICE, 1);
  
  LOG_ENTER(MAC);
  LOG_I(EMU, "3 Starting OAI logs version %s Build date: %s on %s\n", 
	       BUILD_VERSION, BUILD_DATE, BUILD_HOST);  
  LOG_D(MAC, "3 debug  MAC \n");
  LOG_N(MAC, "3 notice MAC \n");
  LOG_W(MAC, "3 warning MAC \n");
  LOG_I(MAC, "3 info MAC \n");
  
  set_comp_log(MAC, LOG_DEBUG,1);
  set_comp_log(EMU, LOG_DEBUG,1);
 
  LOG_ENTER(MAC);
  LOG_I(EMU, "4 Starting OAI logs version %s Build date: %s on %s\n", 
	       BUILD_VERSION, BUILD_DATE, BUILD_HOST);  
  LOG_D(MAC, "4 debug  MAC \n");
  LOG_N(MAC, "4 notice MAC \n");
  LOG_W(MAC, "4 warning MAC \n");
  LOG_I(MAC, "4 info MAC \n");

 
  set_comp_log(MAC, LOG_DEBUG,0);
  set_comp_log(EMU, LOG_DEBUG,0);
 
  LOG_I(LOG, "5 Starting OAI logs version %s Build date: %s on %s\n", 
	       BUILD_VERSION, BUILD_DATE, BUILD_HOST);  
  LOG_D(MAC, "5 debug  MAC \n");
  LOG_N(MAC, "5 notice MAC \n");
  LOG_W(MAC, "5 warning MAC \n");
  LOG_I(MAC, "5 info MAC \n");
  
 
  set_comp_log(MAC, LOG_TRACE,0X07F);
  set_comp_log(EMU, LOG_TRACE,0X07F);
  
  LOG_ENTER(MAC);
  LOG_I(LOG, "6 Starting OAI logs version %s Build date: %s on %s\n", 
	BUILD_VERSION, BUILD_DATE, BUILD_HOST);  
  LOG_D(MAC, "6 debug  MAC \n");
  LOG_N(MAC, "6 notice MAC \n");
  LOG_W(MAC, "6 warning MAC \n");
  LOG_I(MAC, "6 info MAC \n");
  LOG_EXIT(MAC);

}
#endif
